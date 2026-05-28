#pragma once

void button_press_callback();

class ButtonHandler {
public:
  static ButtonHandler& getInstance() {
    static ButtonHandler instance;
    return instance;
  }

  void begin() {
    pinMode(BTN_BUILTIN, INPUT_PULLUP);
    attachInterrupt(BTN_BUILTIN, &button_press_callback, FALLING);
  }

  void task() {
    if (!this->button_pressed) {
      return;
    }
    this->button_pressed = false;
    log.log(Log::INFO, F("Button pressed"));

    // Ignore button presses before the initial time sync
    if (!TimesyncHandler::getInstance().isInitialTimeUpdateDone()) {
      return;
    }

    // Cancel any ongoing text scrolling
    DisplayHandler& display_handler = DisplayHandler::getInstance();
    display_handler.stop_text_scroll();

    // Request a display wake is it's off
    if (!lm.getDisplayOn()) {
      display_handler.request_display_wake();
      return;
    }

    // Request the next clock style if the display is on
    display_handler.request_next_clock_style();
  }

  void register_button_press() {
    this->button_pressed = true;
  }

private:
  bool button_pressed;
  Log& log = Log::getInstance();

  ButtonHandler()
      : button_pressed(false)
      , log(Log::getInstance()) {}

  ButtonHandler(const ButtonHandler&) = delete;
  ButtonHandler& operator=(const ButtonHandler&) = delete;
};

void button_press_callback() {
  static uint32_t btn_last_press = 0;
  if (millis() < btn_last_press + 200) {
    return;
  }
  btn_last_press = millis();
  ButtonHandler::getInstance().register_button_press();
}
