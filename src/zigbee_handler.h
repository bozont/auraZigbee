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
