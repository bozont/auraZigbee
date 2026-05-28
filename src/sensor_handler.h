#pragma once

static ModulinoThermo modulino_thermo;
static LTR381RGBClass modulino_light(Wire, 0x53);

class SensorHandler {
public:
  static SensorHandler& getInstance() {
    static SensorHandler instance;
    return instance;
  }

  void begin() {
    Modulino.begin();
    modulino_thermo.begin();
    modulino_light.begin();
  }

  void task() {
    static uint32_t last_action_temp = 0;
    if ((last_action_temp + 5000) < millis()) {
      last_action_temp = millis();
      this->modulino_thermo_measure();
    }

    this->modulino_light_measure();
  }

  void modulino_thermo_measure() {
    // Drop everything below the first decimal
    float modulino_temp_new = floor(modulino_thermo.getTemperature() * 10) / 10.0 + modulino_temp_calibration;
    // Drop all decimals on humidity
    float modulino_humidity_new = floor(modulino_thermo.getHumidity());

    if (modulino_temp_new != modulino_temp) {
      modulino_temp = modulino_temp_new;
      log.log(Log::INFO, F("Temperature: %.1f C"), modulino_temp);
    }

    if (modulino_humidity_new != modulino_humidity) {
      modulino_humidity = modulino_humidity_new;
      log.log(Log::INFO, F("Humidity: %.1f%%"), modulino_humidity);
    }
  }

  void modulino_light_measure() {
    int light_reading = 0;
    modulino_light.readAmbientLight(light_reading);
    this->modulino_light_level = (float)light_reading;
  }

  float get_temperature() const {
    return this->modulino_temp;
  }

  float get_humidity() const {
    return this->modulino_humidity;
  }

  float get_light_level() const {
    return this->modulino_light_level;
  }

private:
  const float modulino_temp_calibration = -2.0f;
  float modulino_temp;
  float modulino_humidity;
  float modulino_light_level;
  Log& log;

  SensorHandler() :
    modulino_temp(0.0f),
    modulino_humidity(0.0f),
    modulino_light_level(0.0f),
    log(Log::getInstance())
  {}

  SensorHandler(const SensorHandler&) = delete;
  SensorHandler& operator=(const SensorHandler&) = delete;
};
