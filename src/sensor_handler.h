void modulino_thermo_measure() {
  static uint32_t last_action_temp = 0;
  if ((last_action_temp + 5000) < millis()) {
    last_action_temp = millis();

    // Drop everything below the first decimal
    float modulino_temp_new = floor(modulino_thermo.getTemperature() * 10) / 10.0 + modulino_temp_calibration;
    // Drop all decimals on humidity
    float modulino_humidity_new = floor(modulino_thermo.getHumidity());

    if (modulino_temp_new != modulino_temp) {
      modulino_temp = modulino_temp_new;
      zigbee_temp_sensor.set_measured_value_celsius(modulino_temp);
      Serial.printf("Temperature: %.1f C\n", modulino_temp);
    }

    if (modulino_humidity_new != modulino_humidity) {
      modulino_humidity = modulino_humidity_new;
      zigbee_humidity_sensor.set_measured_value_percent(modulino_humidity);
      Serial.printf("Humidity: %.1f%%\n", modulino_humidity);
    }
  }
}
