typedef enum {
  CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP = 0,
  CLOCKSTYLE_SMALL_WITH_SECONDS,
  CLOCKSTYLE_PLASMA,
  CLOCKSTYLE_BIG,
  CLOCKSTYLE_DETAILED_STATUS,
  CLOCKSTYLE_MAX,
  CLOCKSTYLE_LOADING
} clock_styles_t;

static clock_styles_t clock_style = CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP;

void refresh_screen();
void draw_plasma_clock(uint8_t hour, uint8_t minute, int8_t temperature);
void draw_clock(uint8_t hour, uint8_t minute);
void draw_small_clock(uint8_t hour, uint8_t minute, int8_t temperature, bool render_to_screen = true);
void draw_small_clock_with_seconds(uint8_t hour, uint8_t minute, uint8_t seconds);
void show_detailed_status();
void draw_loading_screen();

void refresh_screen() {
  if (!lc.getDisplayOn()) {
    return;
  }

  // Draw the loading screen if the clock is not synced yet
  static bool zigbee_time_initial_update_done_prev = false;
  if (!zigbee_time_initial_update_done) {
    draw_loading_screen();
    return;
  }
  // Clear the screen after the loading animation
  if (zigbee_time_initial_update_done != zigbee_time_initial_update_done_prev) {
    zigbee_time_initial_update_done_prev = zigbee_time_initial_update_done;
    clear_framebuffer();
    render_framebuffer_to_screen();
  }

  MillisRTC& rtc = MillisRTC::getInstance();

  static uint8_t seconds_prev = 61;
  uint8_t second = rtc.get_second();
  if (second != seconds_prev) {
    seconds_prev = second;
    uint8_t hour = rtc.get_hour();
    uint8_t min = rtc.get_minute();

    if (clock_style == CLOCKSTYLE_SMALL_WITH_SECONDS) {
      draw_small_clock_with_seconds(hour, min, second);
    }

    if (clock_style == CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP) {
      static uint8_t min_prev = 61;
      static float temp_prev = -273.0f;
      if (min != min_prev || modulino_temp != temp_prev) {
        min_prev = min;
        temp_prev = modulino_temp;
        draw_small_clock(hour, min, (int8_t)round(modulino_temp));
      }
    }

    static uint8_t min_prev = 61;
    if (min != min_prev) {
      min_prev = min;
      if (clock_style == CLOCKSTYLE_BIG) {
        draw_clock(hour, min);
      }
    }
  }

  if (clock_style == CLOCKSTYLE_PLASMA) {
    draw_plasma_clock(rtc.get_hour(), rtc.get_minute(), (int8_t)round(modulino_temp));
  }

  // Scroll the text if needed - go to the first style when finished
  if (clock_style == CLOCKSTYLE_DETAILED_STATUS) {
    if (!scroll_text()) {
      text_scroll_in_progress = false;
      clock_style = CLOCKSTYLE_SMALL_WITH_INSIDE_TEMP;
      uint8_t hour = rtc.get_hour();
      uint8_t min = rtc.get_minute();
      draw_small_clock(hour, min, (int8_t)round(modulino_temp));
    } else {
      delay(7);
    }
  }
}

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
  static const uint8_t WIDTH = 32u;
  static const uint8_t HEIGHT = 8u;

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
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      float value = sin(x * 0.15f * dirX + t) + sin(y * 0.3f * dirY + t * 1.3f) + sin((x + y) * 0.08f + t * 0.7f);
      bool pixel = (value > 0.3f); // lower threshold = bigger blobs
      // If there's already a pixel of the clock underneath - invert the pixel so the clock stays visible
      if (getxy_framebuffer(x, y)) {
        setxy_framebuffer(x, y, !pixel);
      } else {
        setxy_framebuffer(x, y, pixel);
      }
    }
  }

  t += speed;
  render_framebuffer_to_screen();
}

void draw_clock(uint8_t hour, uint8_t minute) {
  uint8_t hour_tens = hour / 10;
  uint8_t hour_ones = hour % 10;
  uint8_t minute_tens = minute / 10;
  uint8_t minute_ones = minute % 10;

  uint8_t display_x_offset = 3u;
  clear_framebuffer();
  if (hour_tens > 0) {
    draw_num(display_x_offset, 0, hour_tens);
  }
  draw_num(display_x_offset + 6, 0, hour_ones);
  draw_num(display_x_offset + 11, 0, LEDMATRIX_ICON_COLON);
  draw_num(display_x_offset + 14, 0, minute_tens);
  draw_num(display_x_offset + 20, 0, minute_ones);
  render_framebuffer_to_screen();
}

void draw_small_clock(uint8_t hour, uint8_t minute, int8_t temperature, bool render_to_screen) {
  uint8_t hour_tens = hour / 10;
  uint8_t hour_ones = hour % 10;
  uint8_t minute_tens = minute / 10;
  uint8_t minute_ones = minute % 10;

  uint8_t display_x_offset = 1u;
  uint8_t display_y_offset = 2u;

  clear_framebuffer();
  if (hour_tens > 0) {
    draw_small_num(display_x_offset, display_y_offset, hour_tens);
  }
  draw_small_num(display_x_offset + 4, display_y_offset, hour_ones);
  draw_small_num(display_x_offset + 7, display_y_offset, LEDMATRIX_SMALL_ICON_COLON);
  draw_small_num(display_x_offset + 10, display_y_offset, minute_tens);
  draw_small_num(display_x_offset + 14, display_y_offset, minute_ones);

  if (temperature == -127) {
    draw_small_num(23, display_y_offset, LEDMATRIX_SMALL_ICON_DASH);
    draw_small_num(27, display_y_offset, LEDMATRIX_SMALL_ICON_DASH);
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
    draw_small_num(23, display_y_offset, temperature_tens);
    draw_small_num(27, display_y_offset, temperature_ones);
    draw_small_num(30, display_y_offset, LEDMATRIX_SMALL_ICON_CELSIUS);
    if (temp_is_negative) {
      setxy_framebuffer(20, 4, true);
      setxy_framebuffer(21, 4, true);
    }
  } else {
    draw_small_num(27, display_y_offset, temperature_ones);
    draw_small_num(30, display_y_offset, LEDMATRIX_SMALL_ICON_CELSIUS);
    if (temp_is_negative) {
      setxy_framebuffer(24, 4, true);
      setxy_framebuffer(25, 4, true);
    }
  }

  if (render_to_screen) render_framebuffer_to_screen();
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

  clear_framebuffer();
  if (hour_tens > 0) {
    draw_small_num(display_x_offset, display_y_offset, hour_tens);
  }
  draw_small_num(display_x_offset + 4, display_y_offset, hour_ones);
  draw_small_num(display_x_offset + 7, display_y_offset, LEDMATRIX_SMALL_ICON_COLON);
  draw_small_num(display_x_offset + 10, display_y_offset, minute_tens);
  draw_small_num(display_x_offset + 14, display_y_offset, minute_ones);
  draw_small_num(display_x_offset + 17, display_y_offset, LEDMATRIX_SMALL_ICON_COLON);
  draw_small_num(display_x_offset + 20, display_y_offset, second_tens);
  draw_small_num(display_x_offset + 24, display_y_offset, second_ones);
  render_framebuffer_to_screen();
}

void show_detailed_status() {
  MillisRTC& rtc = MillisRTC::getInstance();
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
             modulino_temp,
             modulino_humidity,
             light_level,
             Zigbee.getPanId(),
             Zigbee.getChannel());
  Serial.println(dest);
  setup_text_scroll((String)dest);
  text_scroll_in_progress = true;
}

void draw_loading_screen() {
  static uint8_t wait_cycles = 20u;
  static const uint8_t anim_x_offset = 14u;
  static const uint8_t anim_y_offset = 4u;

  if(wait_cycles != 0) {
      wait_cycles--;
      return;
  } else {
      wait_cycles = 20u;
  }

  static bool led_states[4] = {0,0,0,0};
  static uint8_t anim_cnt = 0;

  if(anim_cnt < 4) {
      led_states[anim_cnt] = true;
  } else if(anim_cnt < 8) {
      led_states[anim_cnt-4] = false;
  }

  anim_cnt++;
  if(anim_cnt == 8) anim_cnt = 0;

  setxy_framebuffer(anim_x_offset  , anim_y_offset, led_states[0]);
  setxy_framebuffer(anim_x_offset+1, anim_y_offset, led_states[1]);
  setxy_framebuffer(anim_x_offset+2, anim_y_offset, led_states[2]);
  setxy_framebuffer(anim_x_offset+3, anim_y_offset, led_states[3]);
  render_framebuffer_to_screen();
}
