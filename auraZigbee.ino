#include <Arduino_Modulino.h>
#include <SPI.h>
#include <WatchdogTimer.h>
#include <bozontlabsMAX7219.h>
#include <chrono>
#include <uptime.h>
#include <vector>

#include <Zigbee.h>
#include <ZigbeeHumiditySensor.h>
#include <ZigbeeLightSensor.h>
#include <ZigbeeLightbulb.h>
#include <ZigbeeTemperatureSensor.h>
#include <ZigbeeTimeClient.h>
#include <time.h>

#include "src/millis_rtc.h"

void draw_small_clock(uint8_t hour, uint8_t minute, int8_t temperature, bool render_to_screen = true);

#define NTP_SYNC_FREQUENCY_HOURS 1

ModulinoThermo modulino_thermo;
LTR381RGBClass modulino_light(Wire, 0x53);

const float modulino_temp_calibration = -2.0f;
float modulino_temp = 0.0f;
float modulino_humidity = 0.0f;
float light_level = 0.0f;

#define LC_SPI_CS A0
#define LC_NUMBER_OF_DEVICES 4
bozontlabsMAX7219 lc = bozontlabsMAX7219(LC_SPI_CS, LC_NUMBER_OF_DEVICES);

ZigbeeTimeClient zigbee_time;
ZigbeeTemperatureSensor zigbee_temp_sensor;
ZigbeeHumiditySensor zigbee_humidity_sensor;
ZigbeeLightSensor zigbee_light_sensor;
ZigbeeLightbulb zigbee_bulb;

volatile bool zigbee_time_initial_update_done = false;
volatile bool zigbee_time_updated = false;
volatile bool button_pressed = false;
volatile bool text_scroll_in_progress = false;

bool display_on_autobrightness = true;
bool display_wake_requested = false;

#include "src/ledmatrix_font.h"
#include "src/ledmatrix_gfx.h"
#include "src/timesync_handler.h"
#include "src/sensor_handler.h"
#include "src/display_content_handler.h"
#include "src/button_handler.h"
#include "src/display_autobrightness_handler.h"

void onZigbeeTimeUpdated() {
  zigbee_time_updated = true;
}

void setup() {
  WatchdogTimer.begin(WDOG_PERIOD_16_S);
  WatchdogTimer.feed();
  Serial.begin(115200);
  Serial.println("auraSmartClock Zigbee Edition (tm)");
  pinMode(LED_BUILTIN_2, OUTPUT);
  digitalWrite(LED_BUILTIN_2, LED_BUILTIN_INACTIVE);
  pinMode(BTN_BUILTIN, INPUT_PULLUP);
  attachInterrupt(BTN_BUILTIN, &handle_button_press, FALLING);

  Wire.begin();
  Modulino.begin();
  modulino_thermo.begin();
  modulino_light.begin();
  delay(200);

  // Boot the screen
  Serial.println("Initializing display...");
  lc.begin();
  lc.setDisplayOn(true);
  lc.setBrightness(0);
  lc.clearDisplay();
  memset(text_to_show_data, 0u, sizeof(text_to_show_data));
  memset(ledmatrix_framebuffer, false, LEDMATRIX_WIDTH * LEDMATRIX_HEIGHT);
  memset(ledmatrix_framebuffer_onscreen, false, (LEDMATRIX_WIDTH * LEDMATRIX_HEIGHT));
  ledmatrix_ascii_table_init();
  render_text_to_framebuffer("hello", 0, 0);
  render_framebuffer_to_screen();
  Serial.println("Display initialized");
  delay(500);

  // Initialize Zigbee
  Zigbee.setVendorName("Bozont Labs Inc.");
  Zigbee.setProductName("auraZigbee");
  Zigbee.setFirmwareVersion(0x00000420);
  zigbee_time.setTimeUpdateCallback(onZigbeeTimeUpdated);
  Zigbee.begin();
  zigbee_bulb.begin();
  zigbee_bulb.set_onoff(true);
  zigbee_time.begin();
  zigbee_temp_sensor.begin();
  zigbee_humidity_sensor.begin();
  zigbee_light_sensor.begin();
  zigbee_light_sensor.set_min_value_lux(0.0f);
  zigbee_light_sensor.set_max_value_lux(100000.0f);
  zigbee_light_sensor.set_light_sensor_type(ZigbeeLightSensor::LIGHT_SENSOR_TYPE_CMOS);

  // Perform initial temperature and humidity measurements
  modulino_temp = floor(modulino_thermo.getTemperature() * 10) / 10.0 + modulino_temp_calibration;
  modulino_humidity = floor(modulino_thermo.getHumidity());
  zigbee_temp_sensor.set_measured_value_celsius(modulino_temp);
  zigbee_humidity_sensor.set_measured_value_percent(modulino_humidity);

  // Wait for the Zigbee network to be joined
  Serial.println("Waiting for Zigbee network...");
  if (!Zigbee.isJoinedToNetwork()) {
    digitalWrite(LED_BUILTIN_2, LED_BUILTIN_ACTIVE);
    clear_framebuffer();
    draw_num(0, 0, LEDMATRIX_ICON_WIFI);
    render_framebuffer_to_screen();
  }
  while (!Zigbee.isJoinedToNetwork()) {
    draw_loading_screen();
    delay(5);
    WatchdogTimer.feed();
  }
  digitalWrite(LED_BUILTIN_2, LED_BUILTIN_INACTIVE);
  Serial.printf("Joined Zigbee network; PAN_ID='0x%02x' channel='%u'\n", Zigbee.getPanId(), Zigbee.getChannel());

  WatchdogTimer.feed();

  clear_framebuffer();
  render_framebuffer_to_screen();
}

void loop() {
  WatchdogTimer.feed();

  uint32_t millis_curr = millis();
  static uint32_t task_2ms_timer = 0;
  if (task_2ms_timer + 2u < millis_curr) {
    task_2ms_timer = millis_curr;
    refresh_screen();
    MillisRTC::getInstance().update();
    handle_button_actions();
  }

  if (text_scroll_in_progress) {
    return;
  }

  static uint32_t task_100ms_timer = 0;
  if (task_100ms_timer + 100u < millis_curr) {
    task_100ms_timer = millis_curr;
    ntpTask();
    display_auto_brightness_task(true);
    modulino_thermo_measure();
    handle_zigbee_identify();
    Uptime::getInstance().update();
    display_onoff_handler_task();
  }

  static uint32_t task_1min_timer = 0;
  if (task_1min_timer + 60000u < millis_curr) {
    task_1min_timer = millis_curr;
    Serial.printf("Uptime: %s\n", Uptime::getInstance().get_uptime_string().c_str());
  }
}

void handle_zigbee_identify() {
  static bool identification_in_progress_last = false;
  bool identification_in_progress = zigbee_bulb.get_identify_in_progress();
  // Handle the identification enable/disable transitions
  if (identification_in_progress && !identification_in_progress_last) {
    Serial.println("Identification started");
  } else if (!identification_in_progress && identification_in_progress_last) {
    // Set the LED to the current on/off state once the identification ends
    Serial.println("Identification ended");
    digitalWrite(LED_BUILTIN_2, LED_BUILTIN_INACTIVE);
  }
  identification_in_progress_last = identification_in_progress;

  // Blink the LED while the identification is in progress
  if (identification_in_progress) {
    if (millis() % 1000 < 500) {
      digitalWrite(LED_BUILTIN_2, LED_BUILTIN_INACTIVE);
    } else {
      digitalWrite(LED_BUILTIN_2, LED_BUILTIN_ACTIVE);
    }
  }
}

void display_onoff_handler_task() {
  static bool display_on_zigbee_prev = true;
  bool display_on_zigbee = zigbee_bulb.get_onoff();
  static bool display_on_autobrightness_prev = true;

  static uint32_t display_wake_start = 0u;
  static bool display_wake_timer_running = false;

  if (display_wake_requested) {
    display_wake_requested = false;
    // If the display is already on we don't need to do anything
    if (lc.getDisplayOn()) {
      return;
    }
    lc.setDisplayOn(true);
    zigbee_bulb.set_onoff(true);
    display_on_zigbee_prev = true;
    Serial.println("Screen ON (Button)");
    display_wake_start = millis();
    display_wake_timer_running = true;
    return;
  }

  if (display_wake_timer_running && ((millis() - display_wake_start) > 10000u)) {
    Serial.println("Screen OFF (Wake timer)");
    display_wake_timer_running = false;
    lc.setDisplayOn(false);
    zigbee_bulb.set_onoff(false);
    display_on_zigbee_prev = false;
    return;
  }

  if (display_on_zigbee != display_on_zigbee_prev) {
    display_on_zigbee_prev = display_on_zigbee;
    display_wake_timer_running = false;
    lc.setDisplayOn(display_on_zigbee);
    if (display_on_zigbee) {
      Serial.println("Screen ON (Zigbee)");
    } else {
      Serial.println("Screen OFF (Zigbee)");
    }
    return;
  }

  // Initial cooldown on the autobrightness on/off to let the display stay on during startup even in complete darkness
  static bool initial_cooldown = true;
  if(initial_cooldown && millis() < 10000) {
    return;
  }
  initial_cooldown = false;

  if (display_on_autobrightness != display_on_autobrightness_prev) {
    display_on_autobrightness_prev = display_on_autobrightness;
    display_wake_timer_running = false;
    lc.setDisplayOn(display_on_autobrightness);
    if (display_on_autobrightness) {
      Serial.println("Screen ON (Autobrightness)");
    } else {
      Serial.println("Screen OFF (Autobrightness)");
    }
    // Sync with Zigbee
    zigbee_bulb.set_onoff(display_on_autobrightness);
    display_on_zigbee_prev = display_on_autobrightness;
  }
}
