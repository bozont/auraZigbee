#pragma once

class DisplayHandler {
public:
  typedef enum {
    CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP = 0,
    CLOCKSTYLE_SMALL_WITH_SECONDS,
    CLOCKSTYLE_PLASMA,
    CLOCKSTYLE_BIG,
    CLOCKSTYLE_DETAILED_STATUS,
    CLOCKSTYLE_USER_MAX,
    CLOCKSTYLE_LOADING,
    CLOCKSTYLE_ZIGBEE_IDENTIFY,
  } clock_styles_t;

  static DisplayHandler& getInstance() {
    static DisplayHandler instance;
    return instance;
  }

  void begin() {
    ledmatrix_gfx.render_text_to_framebuffer("hello", 0, 0);
    ledmatrix_gfx.render_framebuffer_to_screen();
  }

  void task() {
    static uint32_t last_action_refresh = 0;
    if ((last_action_refresh + this->screen_refresh_delay) <= millis()) {
      last_action_refresh = millis();
      this->refresh_screen();
    }

    static uint32_t last_action_onoff = 0;
    if ((last_action_onoff + 200) < millis()) {
      last_action_onoff = millis();
      this->display_onoff_handler();
    }
  }

  void request_next_clock_style() {
    this->clock_style = (clock_styles_t)((int)this->clock_style + 1);
    if (this->clock_style >= CLOCKSTYLE_USER_MAX) {
      this->clock_style = (clock_styles_t)0;
    }
    log.log(Log::INFO, F("Clock style: %u"), this->clock_style);
    return;
  }

  void request_clock_style(clock_styles_t clock_style) {
    this->clock_style = clock_style;
    if (clock_style == CLOCKSTYLE_USER_MAX) {
      this->clock_style = (clock_styles_t)0;
    }
  }

  bool get_text_scroll_in_progress() {
    return this->text_scroll_in_progress;
  }

  void stop_text_scroll() {
    this->text_scroll_in_progress = false;
  }

  void refresh_screen() {
    if (!lm.getDisplayOn()) {
      return;
    }

    MillisRTC& rtc = MillisRTC::getInstance();
    static uint8_t seconds_prev = 61;
    uint8_t second = rtc.get_second();

    // Switching from one clock style to another
    static clock_styles_t clock_style_prev = CLOCKSTYLE_USER_MAX;
    if (clock_style_prev != clock_style) {
      clock_style_prev = clock_style;
      uint8_t hour = rtc.get_hour();
      uint8_t min = rtc.get_minute();
      seconds_prev = 61;

      switch (clock_style) {
        case CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP:
          this->screen_refresh_delay = 50;
          draw_small_clock(hour, min, (int8_t)round(SensorHandler::getInstance().get_temperature()));
          break;

        case CLOCKSTYLE_SMALL_WITH_SECONDS:
          this->screen_refresh_delay = 5;
          draw_small_clock_with_seconds(hour, min, second);
          break;

        case CLOCKSTYLE_PLASMA:
          this->screen_refresh_delay = 0;
          draw_plasma_clock(hour, min, (int8_t)round(SensorHandler::getInstance().get_temperature()));
          break;

        case CLOCKSTYLE_BIG:
          this->screen_refresh_delay = 50;
          draw_clock(hour, min);
          break;

        case CLOCKSTYLE_DETAILED_STATUS:
          this->screen_refresh_delay = 20;
          show_detailed_status();
          break;

        case CLOCKSTYLE_LOADING:
          this->screen_refresh_delay = 2;
          draw_loading_screen();
          break;

        case CLOCKSTYLE_ZIGBEE_IDENTIFY:
          ledmatrix_gfx.setup_text_scroll("     I identify myself as a Zigbee clock. Look at me! Look at meeeee!");
          text_scroll_in_progress = true;
          this->screen_refresh_delay = 20;
          break;

        default:
          break;
      }

      return;
    }

    static bool zigbee_identify_in_progress_prev = false;
    static clock_styles_t clock_style_before_zigbee_identify = CLOCKSTYLE_USER_MAX;

    // Refreshing the current clock style
    switch (clock_style) {
      case CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP: {
        if (second != seconds_prev) {
          seconds_prev = second;
          SensorHandler& sensor_handler = SensorHandler::getInstance();
          static uint8_t min_prev = 61;
          static float temp_prev = -273.0f;
          uint8_t hour = rtc.get_hour();
          uint8_t min = rtc.get_minute();
          if (min != min_prev || sensor_handler.get_temperature() != temp_prev) {
            min_prev = min;
            temp_prev = sensor_handler.get_temperature();
            draw_small_clock(hour, min, (int8_t)round(sensor_handler.get_temperature()));
          }
        }
      } break;

      case CLOCKSTYLE_SMALL_WITH_SECONDS:
        if (second != seconds_prev) {
          seconds_prev = second;
          uint8_t hour = rtc.get_hour();
          uint8_t min = rtc.get_minute();
          draw_small_clock_with_seconds(hour, min, second);
        }
        break;

      case CLOCKSTYLE_PLASMA:
        draw_plasma_clock(rtc.get_hour(), rtc.get_minute(), (int8_t)round(SensorHandler::getInstance().get_temperature()));
        break;

      case CLOCKSTYLE_BIG:
        if (second != seconds_prev) {
          seconds_prev = second;
          uint8_t hour = rtc.get_hour();
          uint8_t min = rtc.get_minute();
          static uint8_t min_prev = 61;
          if (min != min_prev) {
            min_prev = min;
            draw_clock(hour, min);
          }
        }
        break;

      case CLOCKSTYLE_DETAILED_STATUS:
        if (!ledmatrix_gfx.scroll_text()) {
          text_scroll_in_progress = false;
          clock_style = CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP;
          uint8_t hour = rtc.get_hour();
          uint8_t min = rtc.get_minute();
          draw_small_clock(hour, min, (int8_t)round(SensorHandler::getInstance().get_temperature()));
        }
        break;

      case CLOCKSTYLE_LOADING:
        draw_loading_screen();
        if (TimesyncHandler::getInstance().isInitialTimeUpdateDone()) {
          this->request_clock_style(CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP);
        }
        break;

      case CLOCKSTYLE_ZIGBEE_IDENTIFY:
        if (!ledmatrix_gfx.scroll_text()) {
          zigbee_identify_in_progress_prev = false;
          text_scroll_in_progress = false;
          this->request_clock_style(clock_style_before_zigbee_identify);
        }
        break;

      default:
        break;
    }

    bool zigbee_identify_in_progress = ZigbeeHandler::getInstance().get_identify_in_progress();
    if (zigbee_identify_in_progress && !zigbee_identify_in_progress_prev) {
      zigbee_identify_in_progress_prev = true;
      clock_style_before_zigbee_identify = clock_style;
      this->request_clock_style(CLOCKSTYLE_ZIGBEE_IDENTIFY);
    }
  }

  void display_onoff_handler() {
    static bool display_on_zigbee_prev = true;
    bool display_on_zigbee = zigbee_bulb.get_onoff();
    static bool display_on_autobrightness_prev = true;

    static uint32_t display_wake_start = 0u;
    static bool display_wake_timer_running = false;

    if (display_wake_requested) {
      display_wake_requested = false;
      // If the display is already on we don't need to do anything
      if (lm.getDisplayOn()) {
        return;
      }
      lm.setDisplayOn(true);
      zigbee_bulb.set_onoff(true);
      display_on_zigbee_prev = true;
      log.log(Log::INFO, F("Screen ON (Button)"));
      display_wake_start = millis();
      display_wake_timer_running = true;
      return;
    }

    if (display_wake_timer_running && ((millis() - display_wake_start) > 10000u)) {
      log.log(Log::INFO, F("Screen OFF (Wake timer)"));
      display_wake_timer_running = false;
      lm.setDisplayOn(false);
      zigbee_bulb.set_onoff(false);
      display_on_zigbee_prev = false;
      return;
    }

    if (display_on_zigbee != display_on_zigbee_prev) {
      display_on_zigbee_prev = display_on_zigbee;
      display_wake_timer_running = false;
      lm.setDisplayOn(display_on_zigbee);
      if (display_on_zigbee) {
        log.log(Log::INFO, F("Screen ON (Zigbee)"));
      } else {
        log.log(Log::INFO, F("Screen OFF (Zigbee)"));
      }
      return;
    }

    // Initial cooldown on the autobrightness on/off to let the display stay on during startup even in complete darkness
    static bool initial_cooldown = true;
    if (initial_cooldown && millis() < 10000) {
      return;
    }
    initial_cooldown = false;

    bool display_on_autobrightness = DisplayAutobrightnessHandler::getInstance().get_display_on_autobrightness();
    if (display_on_autobrightness != display_on_autobrightness_prev) {
      display_on_autobrightness_prev = display_on_autobrightness;
      display_wake_timer_running = false;
      lm.setDisplayOn(display_on_autobrightness);
      if (display_on_autobrightness) {
        log.log(Log::INFO, F("Screen ON (Autobrightness)"));
      } else {
        log.log(Log::INFO, F("Screen OFF (Autobrightness)"));
      }
      // Sync with Zigbee
      zigbee_bulb.set_onoff(display_on_autobrightness);
      display_on_zigbee_prev = display_on_autobrightness;
    }
  }

  void request_display_wake() {
    this->display_wake_requested = true;
  }

private:
  clock_styles_t clock_style;
  bool text_scroll_in_progress;
  bool display_wake_requested;
  uint32_t screen_refresh_delay;

  void draw_plasma_clock(uint8_t hour, uint8_t minute, int8_t temperature) {
    uint32_t millis_curr = millis();
    static uint32_t refresh_timer = 0;
    if (millis_curr - refresh_timer < 40) {
      return;
    }
    refresh_timer = millis_curr;

    // Draw the small clock without rendering it to the screen
    draw_small_clock(hour, minute, temperature, false);

    // Overlay the plasma effect
    static const uint8_t WIDTH = LedmatrixGFX::LEDMATRIX_WIDTH;
    static const uint8_t HEIGHT = LedmatrixGFX::LEDMATRIX_HEIGHT;

    static float t = 0.0f;
    static float dirX = 1.0f;       // current X multiplier
    static float dirY = 1.0f;       // current Y multiplier
    static float targetDirX = 1.0f; // target direction to interpolate to
    static float targetDirY = 1.0f;

    static int framesSinceChange = 0;

    const float speed = 0.07f;      // plasma motion speed
    const int changeInterval = 120; // frames before choosing a new target direction
    const float smoothing = 0.02f;  // how fast dirX/dirY interpolate

    // Occasionally choose a new target direction
    framesSinceChange++;
    if (framesSinceChange >= changeInterval) {
      framesSinceChange = 0;

      // Target values between -1.0 and 1.0, slightly randomized
      targetDirX = (random(60, 140) / 100.0f) * ((random(0, 2) == 0) ? 1 : -1);
      targetDirY = (random(60, 140) / 100.0f) * ((random(0, 2) == 0) ? 1 : -1);
    }

    // Interpolate current direction toward target
    dirX += (targetDirX - dirX) * smoothing;
    dirY += (targetDirY - dirY) * smoothing;

    // Generate next frame
    uint32_t plasma_pixels = 0u;
    for (int x = 0; x < WIDTH; x++) {
      for (int y = 0; y < HEIGHT; y++) {
        float value = sin(x * 0.15f * dirX + t) + sin(y * 0.3f * dirY + t * 1.3f) + sin((x + y) * 0.08f + t * 0.7f);
        bool pixel = (value > 0.3f); // lower threshold = bigger blobs
        // If there's already a pixel of the clock underneath - invert the pixel so the clock stays visible
        if (ledmatrix_gfx.getxy_framebuffer(x, y)) {
          ledmatrix_gfx.setxy_framebuffer(x, y, !pixel);
        } else {
          ledmatrix_gfx.setxy_framebuffer(x, y, pixel);
          if (pixel) plasma_pixels++;
        }
      }
    }

    t += speed;
    // If there are no visible plasma pixels and the time variable is over 20*pi,
    // set the time variable back by 20*pi to prevent expensive sin calculations slowing down the animation
    if ((plasma_pixels == 0) && (t > (TWO_PI * 10.0f))) {
      t -= (TWO_PI * 10.0f);
    }

    ledmatrix_gfx.render_framebuffer_to_screen();
  }

  void draw_clock(uint8_t hour, uint8_t minute) {
    uint8_t hour_tens = hour / 10;
    uint8_t hour_ones = hour % 10;
    uint8_t minute_tens = minute / 10;
    uint8_t minute_ones = minute % 10;

    uint8_t display_x_offset = 3u;
    ledmatrix_gfx.clear_framebuffer();
    if (hour_tens > 0) {
      ledmatrix_gfx.draw_num(display_x_offset, 0, hour_tens);
    }
    ledmatrix_gfx.draw_num(display_x_offset + 6, 0, hour_ones);
    ledmatrix_gfx.draw_num(display_x_offset + 11, 0, LEDMATRIX_ICON_COLON);
    ledmatrix_gfx.draw_num(display_x_offset + 14, 0, minute_tens);
    ledmatrix_gfx.draw_num(display_x_offset + 20, 0, minute_ones);
    ledmatrix_gfx.render_framebuffer_to_screen();
  }

  void draw_small_clock(uint8_t hour, uint8_t minute, int8_t temperature, bool render_to_screen = true) {
    uint8_t hour_tens = hour / 10;
    uint8_t hour_ones = hour % 10;
    uint8_t minute_tens = minute / 10;
    uint8_t minute_ones = minute % 10;

    uint8_t display_x_offset = 1u;
    uint8_t display_y_offset = 2u;

    ledmatrix_gfx.clear_framebuffer();
    if (hour_tens > 0) {
      ledmatrix_gfx.draw_small_num(display_x_offset, display_y_offset, hour_tens);
    }
    ledmatrix_gfx.draw_small_num(display_x_offset + 4, display_y_offset, hour_ones);
    ledmatrix_gfx.draw_small_num(display_x_offset + 7, display_y_offset, LEDMATRIX_SMALL_ICON_COLON);
    ledmatrix_gfx.draw_small_num(display_x_offset + 10, display_y_offset, minute_tens);
    ledmatrix_gfx.draw_small_num(display_x_offset + 14, display_y_offset, minute_ones);

    if (temperature == -127) {
      ledmatrix_gfx.draw_small_num(23, display_y_offset, LEDMATRIX_SMALL_ICON_DASH);
      ledmatrix_gfx.draw_small_num(27, display_y_offset, LEDMATRIX_SMALL_ICON_DASH);
      return;
    }

    bool temp_is_negative = false;
    if (temperature < 0) {
      temperature = abs(temperature);
      temp_is_negative = true;
    }

    uint8_t temperature_tens = temperature / 10;
    uint8_t temperature_ones = temperature % 10;

    if (temperature_tens > 0) {
      ledmatrix_gfx.draw_small_num(23, display_y_offset, temperature_tens);
      ledmatrix_gfx.draw_small_num(27, display_y_offset, temperature_ones);
      ledmatrix_gfx.draw_small_num(30, display_y_offset, LEDMATRIX_SMALL_ICON_CELSIUS);
      if (temp_is_negative) {
        ledmatrix_gfx.setxy_framebuffer(20, 4, true);
        ledmatrix_gfx.setxy_framebuffer(21, 4, true);
      }
    } else {
      ledmatrix_gfx.draw_small_num(27, display_y_offset, temperature_ones);
      ledmatrix_gfx.draw_small_num(30, display_y_offset, LEDMATRIX_SMALL_ICON_CELSIUS);
      if (temp_is_negative) {
        ledmatrix_gfx.setxy_framebuffer(24, 4, true);
        ledmatrix_gfx.setxy_framebuffer(25, 4, true);
      }
    }

    if (render_to_screen) ledmatrix_gfx.render_framebuffer_to_screen();
  }

  void draw_small_clock_with_seconds(uint8_t hour, uint8_t minute, uint8_t seconds) {
    uint8_t hour_tens = hour / 10;
    uint8_t hour_ones = hour % 10;
    uint8_t minute_tens = minute / 10;
    uint8_t minute_ones = minute % 10;
    uint8_t second_tens = seconds / 10;
    uint8_t second_ones = seconds % 10;

    uint8_t display_x_offset = 2u;
    uint8_t display_y_offset = 2u;

    ledmatrix_gfx.clear_framebuffer();
    if (hour_tens > 0) {
      ledmatrix_gfx.draw_small_num(display_x_offset, display_y_offset, hour_tens);
    }
    ledmatrix_gfx.draw_small_num(display_x_offset + 4, display_y_offset, hour_ones);
    ledmatrix_gfx.draw_small_num(display_x_offset + 7, display_y_offset, LEDMATRIX_SMALL_ICON_COLON);
    ledmatrix_gfx.draw_small_num(display_x_offset + 10, display_y_offset, minute_tens);
    ledmatrix_gfx.draw_small_num(display_x_offset + 14, display_y_offset, minute_ones);
    ledmatrix_gfx.draw_small_num(display_x_offset + 17, display_y_offset, LEDMATRIX_SMALL_ICON_COLON);
    ledmatrix_gfx.draw_small_num(display_x_offset + 20, display_y_offset, second_tens);
    ledmatrix_gfx.draw_small_num(display_x_offset + 24, display_y_offset, second_ones);
    ledmatrix_gfx.render_framebuffer_to_screen();
  }

  void draw_loading_screen() {
    static uint8_t wait_cycles = 20u;
    static const uint8_t anim_x_offset = 14u;
    static const uint8_t anim_y_offset = 4u;

    if (wait_cycles != 0) {
      wait_cycles--;
      return;
    } else {
      wait_cycles = 20u;
    }

    static bool led_states[4] = {0, 0, 0, 0};
    static uint8_t anim_cnt = 0;

    if (anim_cnt < 4) {
      led_states[anim_cnt] = true;
    } else if (anim_cnt < 8) {
      led_states[anim_cnt - 4] = false;
    }

    anim_cnt++;
    if (anim_cnt == 8) anim_cnt = 0;

    ledmatrix_gfx.setxy_framebuffer(anim_x_offset, anim_y_offset, led_states[0]);
    ledmatrix_gfx.setxy_framebuffer(anim_x_offset + 1, anim_y_offset, led_states[1]);
    ledmatrix_gfx.setxy_framebuffer(anim_x_offset + 2, anim_y_offset, led_states[2]);
    ledmatrix_gfx.setxy_framebuffer(anim_x_offset + 3, anim_y_offset, led_states[3]);
    ledmatrix_gfx.render_framebuffer_to_screen();
  }

  void show_detailed_status() {
    MillisRTC& rtc = MillisRTC::getInstance();
    SensorHandler& sensor_handler = SensorHandler::getInstance();
    char dest[300];
    size_t size = sizeof(dest);
    snprintf_P(dest,
               size,
               PSTR("auraZigbee  %u. %s %02u. %s  Uptime: %s  "
                    "Temperature: %.1f°C  Humidity: %.0f%%  Light level: %.0f lx  "
                    "Zigbee PAN ID: 0x%02x  Zigbee channel: %u"),
               rtc.get_year(),
               rtc.get_month_str(),
               rtc.get_day(),
               rtc.get_dayname_str(),
               Uptime::getInstance().get_uptime_string().c_str(),
               sensor_handler.get_temperature(),
               sensor_handler.get_humidity(),
               sensor_handler.get_light_level(),
               Zigbee.getPanId(),
               Zigbee.getChannel());
    ledmatrix_gfx.setup_text_scroll((String)dest);
    text_scroll_in_progress = true;
  }

  DisplayHandler()
      : clock_style(CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP)
      , text_scroll_in_progress(false)
      , display_wake_requested(false)
      , screen_refresh_delay(10)
      , ledmatrix_gfx(LedmatrixGFX::getInstance())
      , log(Log::getInstance()) {}

  LedmatrixGFX& ledmatrix_gfx;
  Log& log = Log::getInstance();

  DisplayHandler(const DisplayHandler&) = delete;
  DisplayHandler& operator=(const DisplayHandler&) = delete;
};
