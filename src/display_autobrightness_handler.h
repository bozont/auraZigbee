#pragma once

class DisplayAutobrightnessHandler {
public:
  static DisplayAutobrightnessHandler& getInstance() {
    static DisplayAutobrightnessHandler instance;
    return instance;
  }

  void begin() {
    ;
  }

  void task() {
    const uint32_t turn_off_threshold_lx = 1.0f;
    const uint32_t turn_on_threshold_lx = 2.0f;
    const uint32_t display_brightness_min_lx = 0.0f;
    const uint32_t display_brightness_max_lx = 2500.0f;
    const uint32_t display_brightness_min_unit = 0u;
    const uint32_t display_brightness_max_unit = 15u;
    const uint8_t light_value_history_size = 20u;

    static uint32_t display_brightness_prev = 100u;
    static bool display_on = true;
    static bool first_run = true;
    static std::vector<float> light_value_history;
    float light_value_rolling_avg = 0.0f;

    float light_level = SensorHandler::getInstance().get_light_level();
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

    uint32_t display_brightness =
        map(light_value_rolling_avg, display_brightness_min_lx, display_brightness_max_lx, display_brightness_min_unit, display_brightness_max_unit);
    if (display_brightness > display_brightness_max_unit) {
      display_brightness = display_brightness_max_unit;
    }
    // log.log(Log::INFO, F("Light level: %.02f lx | Average light level: %.02f lx | Display brightness: %u\n"), light_level, light_value_rolling_avg, display_brightness);

    // Turn off screen if it's dark
    if (this->turn_off_in_darkness) {
      if (light_value_rolling_avg <= turn_off_threshold_lx && display_on) {
        display_on_autobrightness = false;
        display_on = false;
        log.log(Log::INFO, F("Autobrightness display OFF; Light level: %.02f lx | Average light level: %.02f lx"), light_level, light_value_rolling_avg);
      } else if (light_value_rolling_avg >= turn_on_threshold_lx && !display_on) {
        display_on_autobrightness = true;
        display_on = true;
        log.log(Log::INFO, F("Autobrightness display ON; Light level: %.02f lx | Average light level: %.02f lx"), light_level, light_value_rolling_avg);
      }
    }

    // Set the calculated brightness if it has changed and the screen is on
    if (display_brightness_prev != display_brightness && display_on) {
      log.log(Log::INFO, F("Light level: %.02f lx | Average light level: %.02f lx | Display brightness: %u"),
                          light_level,
                          light_value_rolling_avg,
                          display_brightness);
      lm.setBrightness(display_brightness);
      display_brightness_prev = display_brightness;
    }
  }

  // Returns whether the autobrightness function requests the display to be on or off
  bool get_display_on_autobrightness() const {
    return this->display_on_autobrightness;
  }

  void set_turn_off_in_darkness(bool turn_off_in_darkness) {
    this->turn_off_in_darkness = turn_off_in_darkness;
  }

private:
  bool turn_off_in_darkness;
  bool display_on_autobrightness;

  Log& log;

  DisplayAutobrightnessHandler()
      : turn_off_in_darkness(true)
      , display_on_autobrightness(true)
      , log(Log::getInstance()) {}

  DisplayAutobrightnessHandler(const DisplayAutobrightnessHandler&) = delete;
  DisplayAutobrightnessHandler& operator=(const DisplayAutobrightnessHandler&) = delete;
};
