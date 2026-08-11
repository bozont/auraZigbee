#pragma once

class Log {
public:
  enum log_level_t {
    ERROR = 0,
    WARNING,
    INFO,
    DEBUG,
  };

  static Log& getInstance() {
    static Log instance;
    return instance;
  }

  void begin() {
    Serial.begin(115200);
  }

  void log(log_level_t log_level, const __FlashStringHelper* fmt, ...) {
    // Print the arguments to the buffer (2x in size to fit the arguments)
    PGM_P fmt_p = reinterpret_cast<PGM_P>(fmt);
    size_t message_size = strlen_P(fmt_p) * 2;

    // Have a minimum buffer size of 100 - for short messages with long arguments
    if (message_size < 100) message_size = 100;

    char message[message_size];
    va_list args;
    va_start(args, fmt);
    vsnprintf_P(message, message_size, fmt_p, args);
    va_end(args);

    // Print the timestamp to a buffer (40 bytes)
    MillisRTC& rtc = MillisRTC::getInstance();
    char timebuf[40];
    snprintf(timebuf,
             sizeof(timebuf),
             "%s %u-%02u-%02d %02u:%02u:%02u",
             rtc.get_day_name_short_str(),
             rtc.get_year(),
             rtc.get_month(),
             rtc.get_day(),
             rtc.get_hour(),
             rtc.get_minute(),
             rtc.get_second());

    // Get the log level character
    char log_level_char;
    switch (log_level) {
    case ERROR:
      log_level_char = 'E';
      break;
    case WARNING:
      log_level_char = 'W';
      break;
    case INFO:
      log_level_char = 'I';
      break;
    case DEBUG:
      log_level_char = 'D';
      break;
    default:
      log_level_char = 'U';
      break;
    }

    // Assemble the output message
    char logmessage_out[LOG_BUFFER_SIZE];
    sprintf(logmessage_out, "%s (%c) %s", timebuf, log_level_char, message);

    // Print it to Serial
    Serial.println(logmessage_out);
  }

private:
  static const uint32_t LOG_BUFFER_SIZE = 200;

  Log() {}

  Log(const Log&) = delete;
  Log& operator=(const Log&) = delete;
};
