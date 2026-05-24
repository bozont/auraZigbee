bool display_on_autobrightness = true;

void display_auto_brightness_task(bool turn_off_in_darkness) {
  const uint32_t turn_off_threshold_lx = 1.0f;
  const uint32_t turn_on_threshold_lx = 2.0f;
  static uint32_t display_brightness_prev = 100u;
  static bool display_on = true;
  static bool first_run = true;
  float light_value_rolling_avg = 0.0f;
  static std::vector<float> light_value_history;
  const uint8_t light_value_history_size = 20u;

  int light_reading = 0;
  modulino_light.readAmbientLight(light_reading);
  light_level = (float)light_reading;
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

  const uint32_t display_brightness_min_lx = 0.0f;
  const uint32_t display_brightness_max_lx = 2500.0f;
  const uint32_t display_brightness_min_unit = 0u;
  const uint32_t display_brightness_max_unit = 15u;
  uint32_t display_brightness = map(light_value_rolling_avg, display_brightness_min_lx, display_brightness_max_lx, display_brightness_min_unit, display_brightness_max_unit);
  if (display_brightness > display_brightness_max_unit) {
    display_brightness = display_brightness_max_unit;
  }
  // Serial.printf("Light level: %.02f lx | Average light level: %.02f lx | Display brightness: %u\n", light_level, light_value_rolling_avg,
  // display_brightness);

  // Turn off screen if it's dark
  if (turn_off_in_darkness) {
    if (light_value_rolling_avg <= turn_off_threshold_lx && display_on) {
      display_on_autobrightness = false;
      display_on = false;
      Serial.printf("Autobrightness display OFF; Light level: %.02f lx | Average light level: %.02f lx\n", light_level, light_value_rolling_avg);
    } else if (light_value_rolling_avg >= turn_on_threshold_lx && !display_on) {
      display_on_autobrightness = true;
      display_on = true;
      Serial.printf("Autobrightness display ON; Light level: %.02f lx | Average light level: %.02f lx\n", light_level, light_value_rolling_avg);
    }
  }

  // Set the calculated brightness if it has changed and the screen is on
  if (display_brightness_prev != display_brightness && display_on) {
    Serial.printf("Light level: %.02f lx | Average light level: %.02f lx | Display brightness: %u\n",
                  light_level,
                  light_value_rolling_avg,
                  display_brightness);
    lc.setBrightness(display_brightness);
    display_brightness_prev = display_brightness;
  }

  // Update the Zigbee light sensor with the rolling average value every 5 seconds
  uint32_t millis_curr = millis();
  static uint32_t zigbee_light_last_update = 0;
  if (zigbee_light_last_update + 5000u < millis_curr) {
    zigbee_light_last_update = millis_curr;
    zigbee_light_sensor.set_measured_value_lux(light_value_rolling_avg);
  }
}
