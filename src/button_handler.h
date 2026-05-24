volatile bool button_pressed = false;

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

  // Cancel any ongoing text scrolling
  text_scroll_in_progress = false;

  // Request a display wake is it's off
  if(!lc.getDisplayOn()) {
    display_wake_requested = true;
    return;
  }

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
    draw_small_clock(hour, min, (int8_t)round(SensorHandler::getInstance().get_temperature()));
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
