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

#define NTP_SYNC_FREQUENCY_HOURS 1

#define LC_SPI_CS A0
#define LC_NUMBER_OF_DEVICES 4
bozontlabsMAX7219 lc = bozontlabsMAX7219(LC_SPI_CS, LC_NUMBER_OF_DEVICES);

ZigbeeTimeClient zigbee_time;
ZigbeeTemperatureSensor zigbee_temp_sensor;
ZigbeeHumiditySensor zigbee_humidity_sensor;
ZigbeeLightSensor zigbee_light_sensor;
ZigbeeLightbulb zigbee_bulb;

#include "src/ledmatrix_font.h"
#include "src/ledmatrix_gfx.h"
#include "src/timesync_handler.h"
#include "src/sensor_handler.h"
#include "src/display_content_handler.h"
#include "src/display_autobrightness_handler.h"
#include "src/display_onoff_handler.h"
#include "src/zigbee_handler.h"
#include "src/button_handler.h"

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
  SensorHandler& sensor_handler = SensorHandler::getInstance();
  sensor_handler.begin();
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
  sensor_handler.modulino_thermo_measure();
  zigbee_temp_sensor.set_measured_value_celsius(sensor_handler.get_temperature());
  zigbee_humidity_sensor.set_measured_value_percent(sensor_handler.get_humidity());

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
    Uptime::getInstance().update();
    ntpTask();
    SensorHandler::getInstance().task();
    display_auto_brightness_task(true);
    display_onoff_handler_task();
    handle_zigbee_identify();
  }

  static uint32_t task_1min_timer = 0;
  if (task_1min_timer + 60000u < millis_curr) {
    task_1min_timer = millis_curr;
    Serial.printf("Uptime: %s\n", Uptime::getInstance().get_uptime_string().c_str());
  }
}
