bool display_wake_requested = false;

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
