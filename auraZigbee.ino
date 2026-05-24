#include <Arduino_Modulino.h>
#include <SPI.h>
#include <WatchdogTimer.h>
#include <bozontlabsMAX7219.h>
#include <chrono>
#include <uptime.h>
#include <vector>

#include <Zigbee.h>
#include <ZigbeeTimeClient.h>
#include <time.h>

#define LC_SPI_CS A0
#define LC_NUMBER_OF_DEVICES 4
bozontlabsMAX7219 lc = bozontlabsMAX7219(LC_SPI_CS, LC_NUMBER_OF_DEVICES);

#include "src/millis_rtc.h"
#include "src/ledmatrix_font.h"
#include "src/ledmatrix_gfx.h"
#include "src/timesync_handler.h"
#include "src/sensor_handler.h"
#include "src/zigbee_handler.h"
#include "src/display_handler.h"
#include "src/display_autobrightness_handler.h"
#include "src/display_onoff_handler.h"
#include "src/button_handler.h"

void setup() {
  WatchdogTimer.begin(WDOG_PERIOD_16_S);
  WatchdogTimer.feed();
  Serial.begin(115200);
  Serial.println("auraSmartClock Zigbee Edition (tm)");
  pinMode(LED_BUILTIN_2, OUTPUT);
  digitalWrite(LED_BUILTIN_2, LED_BUILTIN_INACTIVE);
  ButtonHandler::getInstance().begin();

  Wire.begin();
  SensorHandler& sensor_handler = SensorHandler::getInstance();
  sensor_handler.begin();
  delay(200);

  // Boot the screen
  Serial.println("Initializing display...");
  DisplayHandler& display_handler = DisplayHandler::getInstance();
  display_handler.begin();
  Serial.println("Display initialized");
  delay(500);

  // Initialize Zigbee
  ZigbeeHandler& zigbee_handler = ZigbeeHandler::getInstance();
  zigbee_handler.begin();
  TimesyncHandler& timesync_handler = TimesyncHandler::getInstance();
  timesync_handler.begin();

  // Perform initial temperature and humidity measurements
  sensor_handler.modulino_thermo_measure();
  sensor_handler.modulino_light_measure();
  zigbee_handler.update_sensors();

  // Wait for the Zigbee network to be joined
  Serial.println("Waiting for Zigbee network...");
  if (!Zigbee.isJoinedToNetwork()) {
    digitalWrite(LED_BUILTIN_2, LED_BUILTIN_ACTIVE);
    clear_framebuffer();
    draw_num(0, 0, LEDMATRIX_ICON_WIFI);
    render_framebuffer_to_screen();
  }
  while (!Zigbee.isJoinedToNetwork()) {
    DisplayHandler::getInstance().draw_loading_screen();
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
    DisplayHandler::getInstance().task();
    MillisRTC::getInstance().update();
    ButtonHandler::getInstance().task();
  }

  if (DisplayHandler::getInstance().get_text_scroll_in_progress()) {
    return;
  }

  static uint32_t task_100ms_timer = 0;
  if (task_100ms_timer + 100u < millis_curr) {
    task_100ms_timer = millis_curr;
    Uptime::getInstance().update();
    TimesyncHandler::getInstance().task();
    SensorHandler::getInstance().task();
    DisplayAutobrightnessHandler::getInstance().task();
    display_onoff_handler_task();
    ZigbeeHandler::getInstance().task();
  }

  static uint32_t task_1min_timer = 0;
  if (task_1min_timer + 60000u < millis_curr) {
    task_1min_timer = millis_curr;
    Serial.printf("Uptime: %s\n", Uptime::getInstance().get_uptime_string().c_str());
  }
}
