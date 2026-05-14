#include <Arduino_Modulino.h>
#include <BH1750.h>
#include <SPI.h>
#include <WatchdogTimer.h>
#include <bozontlabsMAX7219.h>
#include <chrono>
#include <uptime.h>
#include <vector>

#include <Zigbee.h>
#include <ZigbeeTimeClient.h>
#include <ZigbeeTemperatureSensor.h>
#include <ZigbeeHumiditySensor.h>
#include <ZigbeeLightSensor.h>
#include <ZigbeeLightbulb.h>
#include <time.h>

#include "src/millis_rtc.h"

void max7219_set_pixel(uint8_t x, uint8_t y, bool value);
void draw_small_clock(uint8_t hour, uint8_t minute, int8_t temperature, bool render_to_screen=true);
#include "src/ledmatrix_font.h"
#include "src/ledmatrix_gfx.h"

#define NTP_SYNC_FREQUENCY_HOURS 1

ModulinoThermo modulino_thermo;
BH1750 lightSensor;

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

volatile bool zigbee_time_updated = false;
volatile bool button_pressed = false;

void onZigbeeTimeUpdated()
{
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
  lightSensor.begin();
  Modulino.begin();
  modulino_thermo.begin();

  Serial.println("Initializing display...");
  delay(200);
  // Boot the screen
  lc.begin();
  display_on_off(true);
  display_set_brightness(0);
  display_clear();
  memset(text_to_show_data, 0u, sizeof(text_to_show_data));
  memset(ledmatrix_framebuffer, false, LEDMATRIX_WIDTH * LEDMATRIX_HEIGHT);
  memset(ledmatrix_framebuffer_onscreen, false, (LEDMATRIX_WIDTH * LEDMATRIX_HEIGHT));
  ledmatrix_ascii_table_init();
  render_text_to_framebuffer("hello", 0, 0);
  render_framebuffer_to_screen();
  Serial.println("Display initialized");

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

  Serial.println("Waiting for Zigbee network...");
  while (!Zigbee.isJoinedToNetwork()) {
    delay(200);
    WatchdogTimer.feed();
  }
  Serial.println("Joined Zigbee network!");
  Serial.print("Channel: ");
  Serial.println(Zigbee.getChannel());
  Serial.print("PAN ID: 0x");
  Serial.println(Zigbee.getPanId(), HEX);

  modulino_temp = floor(modulino_thermo.getTemperature() * 10) / 10.0 + modulino_temp_calibration;
  modulino_humidity= floor(modulino_thermo.getHumidity());

  WatchdogTimer.feed();
}

void loop() {
  WatchdogTimer.feed();
  refresh_screen();
  MillisRTC::getInstance().update();
  handle_button_actions();

  uint32_t millis_curr = millis();
  static uint32_t task_100ms_timer = 0;
  if (task_100ms_timer + 100u < millis_curr) {
    task_100ms_timer = millis_curr;
    ntpTask();
    display_auto_brightness_task(true);
    modulino_thermo_measure();
    handle_zigbee_identify();
    Uptime::getInstance().update();
    handle_zigbee_display_onoff();
  }

  static uint32_t task_1min_timer = 0;
  if (task_1min_timer + 60000u < millis_curr) {
    task_1min_timer = millis_curr;
    Serial.printf("Uptime: %s\n", Uptime::getInstance().get_uptime_string().c_str());
  }
}

void handle_zigbee_display_onoff() {
  // Handle on/off state changes from the network
  static bool bulb_prev_state = false;
  bool bulb_current_state = zigbee_bulb.get_onoff();
  // If the current state is ON and the previous was OFF - turn on the LED
  if (bulb_current_state && !bulb_prev_state) {
    bulb_prev_state = bulb_current_state;
    display_on_off(true);
    Serial.println("Screen ON");
  }
  // If the current state is OFF and the previous was ON - turn off the LED
  if (!bulb_current_state && bulb_prev_state) {
    bulb_prev_state = bulb_current_state;
    display_on_off(false);
    Serial.println("Screen OFF");
  }
}

void modulino_thermo_measure() {
  static uint32_t last_action_temp = 0;
  if ((last_action_temp + 5000) < millis()) {
    last_action_temp = millis();

    // Drop everything below the first decimal
    float modulino_temp_new = floor(modulino_thermo.getTemperature() * 10) / 10.0 + modulino_temp_calibration;
    // Drop all decimals on humidity
    float modulino_humidity_new = floor(modulino_thermo.getHumidity());

    if(modulino_temp_new != modulino_temp) {
      modulino_temp = modulino_temp_new;
      zigbee_temp_sensor.set_measured_value_celsius(modulino_temp);
      Serial.printf("Temperature: %.1f C\n", modulino_temp);
    }

    if(modulino_humidity_new != modulino_humidity) {
      modulino_humidity = modulino_humidity_new;
      zigbee_humidity_sensor.set_measured_value_percent(modulino_humidity);
      Serial.printf("Humidity: %.1f%%\n", modulino_humidity);
    }
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

typedef enum {
  CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP = 0,
  CLOCKSTYLE_SMALL_WITH_SECONDS,
  CLOCKSTYLE_PLASMA,
  CLOCKSTYLE_BIG,
  CLOCKSTYLE_DETAILED_STATUS,
  CLOCKSTYLE_MAX
} clock_styles_t;

static clock_styles_t clock_style = CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP;

void refresh_screen() {
  if(!lc.getDisplayOn()) {
    return;
  }

  MillisRTC& rtc = MillisRTC::getInstance();

  static uint8_t seconds_prev = 61;
  uint8_t second = rtc.get_second();
  if (second != seconds_prev) {
    seconds_prev = second;
    uint8_t hour = rtc.get_hour();
    uint8_t min = rtc.get_minute();
    // Serial.printf("%lu / %lu (max: %lu)\n", getUsedHeapSize(), getTotalHeapSize(), getHeapHighWatermark());

    if (clock_style == CLOCKSTYLE_SMALL_WITH_SECONDS) {
      // printf("%u:%02u:%02u\n", hour, min, second);
      draw_small_clock_with_seconds(hour, min, second);
    }

    
    if (clock_style == CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP) {
      static uint8_t min_prev = 61;
      static float temp_prev = -273.0f;
      if (min != min_prev || modulino_temp != temp_prev) {
        min_prev = min;
        temp_prev = modulino_temp;
        draw_small_clock(hour, min, (int8_t)modulino_temp);
      }
    }

    static uint8_t min_prev = 61;
    if (min != min_prev) {
      min_prev = min;
      // Serial.printf("%u:%02u\n", hour, min);
      if (clock_style == CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP) {
        // draw_small_clock(hour, min, (int8_t)(tempSensor.getTemperature() + temp_sensor_calibration));
      } else if (clock_style == CLOCKSTYLE_BIG) {
        draw_clock(hour, min);
      }
    }
  }

  if (clock_style == CLOCKSTYLE_PLASMA) {
    draw_plasma_clock(rtc.get_hour(), rtc.get_minute(), modulino_temp);
  }

  // Scroll the text if needed - go to the first style when finished
  if (clock_style == CLOCKSTYLE_DETAILED_STATUS) {
    if (!scroll_text()) {
      clock_style = CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP;
      uint8_t hour = rtc.get_hour();
      uint8_t min = rtc.get_minute();
      draw_small_clock(hour, min, modulino_temp);
    } else {
      delay(5);
    }
  }
}

void draw_plasma_clock(uint8_t hour, uint8_t minute, int8_t temperature) {
  uint32_t millis_curr = millis();
  static uint32_t refresh_timer = 0;
  if (millis_curr - refresh_timer < 40) {
    return;
  }
  refresh_timer = millis_curr;

  // Draw the small clock without rendering it to the screen
  draw_small_clock(hour, minute, temperature, false);

  // Overlay the plasma effect
  static const uint8_t WIDTH = 32u;
  static const uint8_t HEIGHT = 8u;

  static float t = 0.0f;
  static float dirX = 1.0f;       // current X multiplier
  static float dirY = 1.0f;       // current Y multiplier
  static float targetDirX = 1.0f; // target direction to interpolate to
  static float targetDirY = 1.0f;

  static int framesSinceChange = 0;

  const float speed = 0.07f;       // plasma motion speed
  const int changeInterval = 120; // frames before choosing a new target direction
  const float smoothing = 0.02f;  // how fast dirX/dirY interpolate

  // Occasionally choose a new target direction
  framesSinceChange++;
  if (framesSinceChange >= changeInterval) {
    framesSinceChange = 0;

    // Target values between -1.0 and 1.0, slightly randomized
    targetDirX = (random(60, 140) / 100.0f) * ((random(0, 2) == 0) ? 1 : -1);
    targetDirY = (random(60, 140) / 100.0f) * ((random(0, 2) == 0) ? 1 : -1);
  }

  // Interpolate current direction toward target
  dirX += (targetDirX - dirX) * smoothing;
  dirY += (targetDirY - dirY) * smoothing;

  // Generate next frame
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      float value = sin(x * 0.15f * dirX + t) + sin(y * 0.3f * dirY + t * 1.3f) + sin((x + y) * 0.08f + t * 0.7f);
      bool pixel = (value > 0.3f); // lower threshold = bigger blobs
      // If there's already a pixel of the clock underneath - invert the pixel so the clock stays visible
      if(getxy_framebuffer(x, y)) {
        setxy_framebuffer(x, y, !pixel);
      } else {
        setxy_framebuffer(x, y, pixel);
      }
    }
  }

  t += speed;
  render_framebuffer_to_screen();
}

void draw_clock(uint8_t hour, uint8_t minute) {
  uint8_t hour_tens = hour / 10;
  uint8_t hour_ones = hour % 10;
  uint8_t minute_tens = minute / 10;
  uint8_t minute_ones = minute % 10;

  uint8_t display_x_offset = 3u;
  clear_framebuffer();
  if (hour_tens > 0) {
    draw_num(display_x_offset, 0, hour_tens);
  }
  draw_num(display_x_offset + 6, 0, hour_ones);
  draw_num(display_x_offset + 11, 0, LEDMATRIX_ICON_COLON);
  draw_num(display_x_offset + 14, 0, minute_tens);
  draw_num(display_x_offset + 20, 0, minute_ones);
  render_framebuffer_to_screen();
}

void draw_small_clock(uint8_t hour, uint8_t minute, int8_t temperature, bool render_to_screen) {
  uint8_t hour_tens = hour / 10;
  uint8_t hour_ones = hour % 10;
  uint8_t minute_tens = minute / 10;
  uint8_t minute_ones = minute % 10;

  uint8_t display_x_offset = 1u;
  uint8_t display_y_offset = 2u;

  clear_framebuffer();
  if (hour_tens > 0) {
    draw_small_num(display_x_offset, display_y_offset, hour_tens);
  }
  draw_small_num(display_x_offset + 4, display_y_offset, hour_ones);
  draw_small_num(display_x_offset + 7, display_y_offset, LEDMATRIX_SMALL_ICON_COLON);
  draw_small_num(display_x_offset + 10, display_y_offset, minute_tens);
  draw_small_num(display_x_offset + 14, display_y_offset, minute_ones);

  if (temperature == -127) {
    draw_small_num(23, display_y_offset, LEDMATRIX_SMALL_ICON_DASH);
    draw_small_num(27, display_y_offset, LEDMATRIX_SMALL_ICON_DASH);
    return;
  }

  bool temp_is_negative = false;
  if (temperature < 0) {
    temperature = abs(temperature);
    temp_is_negative = true;
  }

  uint8_t temperature_tens = temperature / 10;
  uint8_t temperature_ones = temperature % 10;

  if (temperature_tens > 0) {
    draw_small_num(23, display_y_offset, temperature_tens);
    draw_small_num(27, display_y_offset, temperature_ones);
    draw_small_num(30, display_y_offset, LEDMATRIX_SMALL_ICON_CELSIUS);
    if (temp_is_negative) {
      setxy_framebuffer(20, 4, true);
      setxy_framebuffer(21, 4, true);
    }
  } else {
    draw_small_num(27, display_y_offset, temperature_ones);
    draw_small_num(30, display_y_offset, LEDMATRIX_SMALL_ICON_CELSIUS);
    if (temp_is_negative) {
      setxy_framebuffer(24, 4, true);
      setxy_framebuffer(25, 4, true);
    }
  }

  if(render_to_screen) render_framebuffer_to_screen();
}

void draw_small_clock_with_seconds(uint8_t hour, uint8_t minute, uint8_t seconds) {
  uint8_t hour_tens = hour / 10;
  uint8_t hour_ones = hour % 10;
  uint8_t minute_tens = minute / 10;
  uint8_t minute_ones = minute % 10;
  uint8_t second_tens = seconds / 10;
  uint8_t second_ones = seconds % 10;

  uint8_t display_x_offset = 2u;
  uint8_t display_y_offset = 2u;

  clear_framebuffer();
  if (hour_tens > 0) {
    draw_small_num(display_x_offset, display_y_offset, hour_tens);
  }
  draw_small_num(display_x_offset + 4, display_y_offset, hour_ones);
  draw_small_num(display_x_offset + 7, display_y_offset, LEDMATRIX_SMALL_ICON_COLON);
  draw_small_num(display_x_offset + 10, display_y_offset, minute_tens);
  draw_small_num(display_x_offset + 14, display_y_offset, minute_ones);
  draw_small_num(display_x_offset + 17, display_y_offset, LEDMATRIX_SMALL_ICON_COLON);
  draw_small_num(display_x_offset + 20, display_y_offset, second_tens);
  draw_small_num(display_x_offset + 24, display_y_offset, second_ones);
  render_framebuffer_to_screen();
}

void show_detailed_status() {
  char temp_postfix = 'C';
  char dest[200];
  size_t size = sizeof(dest);
  snprintf_P(dest,
             size,
             PSTR("auraZigbee  Uptime: %s  Temperature: %.1f°C  Humidity: %.0f%%  Light level: %.0f lx  Zigbee PAN ID: 0x%02x  Zigbee channel: %u"),
             Uptime::getInstance().get_uptime_string().c_str(),
             modulino_temp,
             modulino_humidity,
             light_level,
             Zigbee.getPanId(),
             Zigbee.getChannel()
            );
  Serial.println(dest);
  setup_text_scroll((String)dest);
}

void handle_button_press() {
  static uint32_t btn_last_press = 0;
  if (millis() < btn_last_press + 200) {
    return;
  }
  btn_last_press = millis();
  button_pressed = true;
}

void handle_button_actions() {
  if (!button_pressed) {
    return;
  }
  button_pressed = false;

  Serial.println("Button pressed");
  clock_style = (clock_styles_t)((int)clock_style + 1);
  if (clock_style >= CLOCKSTYLE_MAX) {
    clock_style = (clock_styles_t)0;
  }
  MillisRTC& rtc = MillisRTC::getInstance();
  uint8_t hour = rtc.get_hour();
  uint8_t min = rtc.get_minute();
  uint8_t second = rtc.get_second();
  if (clock_style == CLOCKSTYLE_SMALL_WITH_SECONDS) {
    draw_small_clock_with_seconds(hour, min, second);
  }
  if (clock_style == CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP) {
    draw_small_clock(hour, min, (int8_t)modulino_temp);
  }
  if (clock_style == CLOCKSTYLE_BIG) {
    draw_clock(hour, min);
  }
  if (clock_style == CLOCKSTYLE_DETAILED_STATUS) {
    show_detailed_status();
  }
  Serial.print("Clock style: ");
  Serial.println(clock_style);
}

void display_auto_brightness_task(bool turn_off_in_darkness) {
  const uint32_t turn_off_threshold_lx = 1.0f;
  const uint32_t turn_on_threshold_lx = 2.0f;
  static uint32_t display_brightness_prev = 100u;
  static bool display_on = true;
  static bool first_run = true;
  float light_value_rolling_avg = 0.0f;
  static std::vector<float> light_value_history;
  const uint8_t light_value_history_size = 20u;

  light_level = lightSensor.readLightLevel();
  if (first_run) {
    for (uint8_t i = 0; i < light_value_history_size; i++) {
      light_value_history.push_back(light_level);
    }
    first_run = false;
  }
  light_value_history.erase(light_value_history.begin());
  light_value_history.push_back(light_level);
  float sum = 0.0f;
  for (float v : light_value_history) {
    sum += v;
  }
  light_value_rolling_avg = sum / light_value_history.size();

  uint32_t display_brightness = light_value_rolling_avg / 100;
  if (display_brightness > 15) {
    display_brightness = 15;
  }
  // Serial.printf("Light level: %.02f lx | Average light level: %.02f lx | Display brightness: %u\n", light_level, light_value_rolling_avg,
  // display_brightness);

  // Turn off screen if it's dark
  if (turn_off_in_darkness) {
    if (light_value_rolling_avg <= turn_off_threshold_lx && display_on) {
      display_on_off(false);
      display_on = false;
      Serial.printf("Display OFF; Light level: %.02f lx | Average light level: %.02f lx\n", light_level, light_value_rolling_avg);
    } else if (light_value_rolling_avg >= turn_on_threshold_lx && !display_on) {
      display_on_off(true);
      display_on = true;
      Serial.printf("Display ON; Light level: %.02f lx | Average light level: %.02f lx\n", light_level, light_value_rolling_avg);
    }
  }

  // Set the calculated brightness if it has changed and the screen is on
  if (display_brightness_prev != display_brightness && display_on) {
    Serial.printf("Light level: %.02f lx | Average light level: %.02f lx | Display brightness: %u\n",
                  light_level,
                  light_value_rolling_avg,
                  display_brightness);
    display_set_brightness(display_brightness);
    display_brightness_prev = display_brightness;
  }

  uint32_t millis_curr = millis();
  static uint32_t zigbee_light_last_update = 0;
  if (zigbee_light_last_update + 5000u < millis_curr) {
    zigbee_light_last_update = millis_curr;
    zigbee_light_sensor.set_measured_value_lux(light_value_rolling_avg);
  }
}

enum ntp_state_e : uint8_t { NTP_INIT = 0, NTP_SYNC_IN_PROGRESS, NTP_SLEEP, NTP_SYNC_FAILED };

void ntpTask() {
  static ntp_state_e state = NTP_INIT;
  static uint32_t last_sync_sec = 0u;
  static const uint32_t SYNC_FAILED_RETRY_TIME_SEC = 10u;

  switch (state) {
  case NTP_INIT:
    state = NTP_SYNC_IN_PROGRESS;
    break;

  case NTP_SYNC_IN_PROGRESS:
    if (syncNTP()) {
      state = NTP_SLEEP;
      last_sync_sec = millis() / 1000;
    } else {
      state = NTP_SYNC_FAILED;
      Serial.printf("Retrying sync in %d seconds...\n", SYNC_FAILED_RETRY_TIME_SEC);
    }
    break;

  case NTP_SYNC_FAILED: {
    static uint32_t last_failed_sync_time = 0u;
    if (millis() - last_failed_sync_time >= (uint32_t)SYNC_FAILED_RETRY_TIME_SEC * 1000u) {
      last_failed_sync_time = millis();
      state = NTP_SYNC_IN_PROGRESS;
    }
  } break;

  case NTP_SLEEP: {
    uint32_t uptime_sec = millis() / 1000;
    uint32_t next_sync_time = last_sync_sec + NTP_SYNC_FREQUENCY_HOURS * 3600u;
    if (uptime_sec >= next_sync_time) {
      state = NTP_SYNC_IN_PROGRESS;
    }
  } break;

  default:
    state = NTP_INIT;
    break;
  }
}

bool syncNTP() {
  Serial.println("Starting Zigbee time sync...");
  zigbee_time.requestTime();

  delay(1000);

  if (zigbee_time_updated) {
    MillisRTC& rtc = MillisRTC::getInstance();
    zigbee_time_updated = false;
    uint32_t current_ntp_time = zigbee_time.getLocalUnixTime();
    rtc.set_from_unix(current_ntp_time);
    Serial.printf("Current NTP time: %lu\n", current_ntp_time);

    time_t time = rtc.get_unix();
    struct tm ts;
    // ts = *localtime(&time);
    gmtime_r(&time, &ts);
    char time_buf[100];
    snprintf(time_buf,
             sizeof(time_buf),
             "%04d-%02d-%02d %02d:%02d:%02d",
             ts.tm_year + 1900,
             ts.tm_mon + 1,
             ts.tm_mday,
             ts.tm_hour,
             ts.tm_min,
             ts.tm_sec);
    Serial.println(time_buf);

    Serial.println("Zigbee time sync finished");
    return true;
  }

  Serial.println("Zigbee time sync failed");
  yield();
  return false;
}

void display_on_off(bool onoff) {
  lc.setDisplayOn(onoff);
}

void display_set_brightness(uint8_t brightness) {
  lc.setBrightness(brightness);
}

void display_clear() {
  lc.clearDisplay();
}

void max7219_set_pixel(uint8_t x, uint8_t y, bool value) {
  if (x >= LEDMATRIX_WIDTH || y >= LEDMATRIX_HEIGHT) {
    return;
  }

  // Only render the pixel if there's a change
  if (ledmatrix_framebuffer_onscreen[x][y] != value) {
    lc.setPixel(x, y, value);
    ledmatrix_framebuffer_onscreen[x][y] = value;
  }
}
