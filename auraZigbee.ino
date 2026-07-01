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

#define LM_SPI_CS A0
#define LM_NUMBER_OF_DEVICES 4
bozontlabsMAX7219 lm = bozontlabsMAX7219(LM_SPI_CS, LM_NUMBER_OF_DEVICES);

#include "src/millis_rtc.h"
#include "src/log.h"
#include "src/ledmatrix_font.h"
#include "src/ledmatrix_gfx.h"
#include "src/timesync_handler.h"
#include "src/sensor_handler.h"
#include "src/zigbee_handler.h"
#include "src/display_autobrightness_handler.h"
#include "src/display_handler.h"
#include "src/button_handler.h"

void setup() {
  WatchdogTimer.begin(WDOG_PERIOD_16_S);
  WatchdogTimer.feed();

  Log& log = Log::getInstance();
  log.begin();
  log.log(Log::INFO, F("auraSmartClock Zigbee Edition (tm)"));
  log.log(Log::INFO, F("Build date: %s %s"), __DATE__, __TIME__);

  pinMode(LED_BUILTIN_2, OUTPUT);
  digitalWrite(LED_BUILTIN_2, LED_BUILTIN_INACTIVE);
  ButtonHandler::getInstance().begin();

  Wire.begin();
  SensorHandler& sensor_handler = SensorHandler::getInstance();
  sensor_handler.begin();
  delay(200);

  // Boot the screen
  log.log(Log::INFO, F("Initializing display..."));
  lm.begin();
  lm.setDisplayOn(true);
  lm.setBrightness(0);
  lm.clearDisplay();
  LedmatrixGFX& ledmatrix_gfx = LedmatrixGFX::getInstance();
  ledmatrix_gfx.begin();
  DisplayHandler& display_handler = DisplayHandler::getInstance();
  display_handler.begin();
  log.log(Log::INFO, F("Display initialized"));
  // Set the initial brightness
  sensor_handler.modulino_light_measure();
  DisplayAutobrightnessHandler& display_autobrigntess_handler = DisplayAutobrightnessHandler::getInstance();
  display_autobrigntess_handler.begin();
  display_autobrigntess_handler.set_turn_off_in_darkness(true);
  display_autobrigntess_handler.task();
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
  ledmatrix_gfx.clear_framebuffer();
  display_handler.request_clock_style(DisplayHandler::CLOCKSTYLE_LOADING);
  if (!Zigbee.isPaired()) {
    log.log(Log::INFO, F("Device is unpaired and entered network pairing mode"));
    digitalWrite(LED_BUILTIN_2, LED_BUILTIN_ACTIVE);
    ledmatrix_gfx.draw_num(0, 0, LEDMATRIX_ICON_WIFI);
    ledmatrix_gfx.render_framebuffer_to_screen();
  }
  log.log(Log::INFO, F("Connecting to Zigbee network..."));
  while (!Zigbee.isConnectedToNetwork()) {
    display_handler.refresh_screen();
    delay(5);
    WatchdogTimer.feed();
  }
  digitalWrite(LED_BUILTIN_2, LED_BUILTIN_INACTIVE);
  log.log(Log::INFO, F("Connected to Zigbee network; PAN_ID='0x%02x' channel='%u'"), Zigbee.getPanId(), Zigbee.getChannel());

  WatchdogTimer.feed();
}

void loop() {
  WatchdogTimer.feed();
  DisplayHandler::getInstance().task();

  uint32_t millis_curr = millis();
  static uint32_t task_2ms_timer = 0;
  if (task_2ms_timer + 2u < millis_curr) {
    task_2ms_timer = millis_curr;
    MillisRTC::getInstance().update();
    ButtonHandler::getInstance().task();
  }

  static uint32_t task_100ms_timer = 0;
  if (task_100ms_timer + 100u < millis_curr) {
    task_100ms_timer = millis_curr;
    Uptime::getInstance().update();
    ZigbeeHandler::getInstance().task();
    if (!DisplayHandler::getInstance().get_text_scroll_in_progress()) {
      TimesyncHandler::getInstance().task();
      SensorHandler::getInstance().task();
      DisplayAutobrightnessHandler::getInstance().task();
    }
  }

  static uint32_t task_1min_timer = 0;
  if (task_1min_timer + 60000u < millis_curr) {
    task_1min_timer = millis_curr;
    Log& log = Log::getInstance();
    log.log(Log::INFO, F("Uptime: %s"), Uptime::getInstance().get_uptime_string().c_str());
  }
}
