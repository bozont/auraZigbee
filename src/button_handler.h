#pragma once

volatile bool button_pressed = false;

void handle_button_press() {
  static uint32_t btn_last_press = 0;
  if (millis() < btn_last_press + 200) {
    return;
  }
  btn_last_press = millis();
  button_pressed = true;
}

class ButtonHandler {
public:
  static ButtonHandler& getInstance() {
    static ButtonHandler instance;
    return instance;
  }

  void begin() {
    pinMode(BTN_BUILTIN, INPUT_PULLUP);
    attachInterrupt(BTN_BUILTIN, &handle_button_press, FALLING);
  }

  void task() {
    if (!button_pressed) {
      return;
    }
    button_pressed = false;
    Serial.println("Button pressed");

    // Ignore button presses before the initial time sync
    if (!TimesyncHandler::getInstance().isInitialTimeUpdateDone()) {
      return;
    }

    // Cancel any ongoing text scrolling
    DisplayHandler& display_handler = DisplayHandler::getInstance();
    display_handler.stop_text_scroll();

    // Request a display wake is it's off
    if (!lc.getDisplayOn()) {
      display_wake_requested = true;
      return;
    }

    // Request the next clock style if the display is on
    display_handler.request_next_clock_style();
  }

private:
  ButtonHandler() {}

  ButtonHandler(const ButtonHandler&) = delete;
  ButtonHandler& operator=(const ButtonHandler&) = delete;
};
