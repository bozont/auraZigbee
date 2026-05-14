
/*
MIT License

bozontlabsMillisRTC - millis() based real time clock library for Arduino devices
Copyright (c) 2026 Tamas Jozsi

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

This permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <Arduino.h>

class MillisRTC {
public:
  static MillisRTC& getInstance() {
    static MillisRTC instance;
    return instance;
  }

  void update() {
    uint32_t now = millis();
    uint32_t elapsed = now - last_millis; // rollover-safe

    if (elapsed >= 1000) {
      uint32_t seconds = elapsed / 1000;

      noInterrupts();
      this->device_rtc_clock += seconds;
      interrupts();

      last_millis += seconds * 1000;
    }
  }

  void set_from_unix(time_t unix_ts) {
    noInterrupts();
    this->device_rtc_clock = unix_ts;
    last_millis = millis();
    interrupts();
  }

  time_t get_unix() {
    return this->device_rtc_clock;
  }

  static uint8_t is_leap_year(uint16_t year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
  }

  uint16_t get_year(void) {
    uint32_t seconds = this->device_rtc_clock;
    uint16_t year = 1970;

    while (1) {
      uint32_t year_seconds = this->is_leap_year(year) ? 31622400UL : 31536000UL;
      if (seconds < year_seconds) {
        break;
      }
      seconds -= year_seconds;
      year++;
    }

    return year;
  }

  uint8_t get_month(void) {
    static const uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    uint32_t seconds = this->device_rtc_clock;
    uint16_t year = 1970;

    // Remove whole years
    while (1) {
      uint32_t year_seconds = this->is_leap_year(year) ? 31622400UL : 31536000UL;
      if (seconds < year_seconds) break;
      seconds -= year_seconds;
      year++;
    }

    // Remove whole months
    for (uint8_t month = 0; month < 12; month++) {
      uint8_t dim = days_in_month[month];
      if (month == 1 && this->is_leap_year(year)) dim++; // February leap

      uint32_t month_seconds = dim * 86400UL;
      if (seconds < month_seconds) {
        return month + 1; // 1–12
      }
      seconds -= month_seconds;
    }

    return 1; // fallback
  }

  uint8_t get_day(void) {
    static const uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    uint32_t seconds = this->device_rtc_clock;
    uint16_t year = 1970;

    // Remove years
    while (1) {
      uint32_t year_seconds = this->is_leap_year(year) ? 31622400UL : 31536000UL;
      if (seconds < year_seconds) break;
      seconds -= year_seconds;
      year++;
    }

    // Remove months
    for (uint8_t month = 0; month < 12; month++) {
      uint8_t dim = days_in_month[month];
      if (month == 1 && this->is_leap_year(year)) dim++;

      uint32_t month_seconds = dim * 86400UL;
      if (seconds < month_seconds) {
        return (seconds / 86400UL) + 1; // 1–31
      }
      seconds -= month_seconds;
    }

    return 1;
  }

  uint8_t get_dayname(void) {
    // 1970-01-01 was a Thursday (4)
    return (uint8_t)((this->device_rtc_clock / 86400UL + 4) % 7);
  }

  const char* get_dayname_str(void) {
    static const char* names[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    return names[this->get_dayname()];
  }

  uint8_t get_second() {
    return this->device_rtc_clock % 60;
  }

  uint8_t get_minute() {
    return (this->device_rtc_clock / 60) % 60;
  }

  uint8_t get_hour() {
    return (this->device_rtc_clock / 3600) % 24;
  }

private:
  uint32_t device_rtc_clock;
  uint32_t last_millis;

  MillisRTC()
      : device_rtc_clock(0u)
      , last_millis(0u) {}

  MillisRTC(const MillisRTC&) = delete;
  MillisRTC& operator=(const MillisRTC&) = delete;
};
