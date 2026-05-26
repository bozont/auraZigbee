#pragma once

#include <ZigbeeHumiditySensor.h>
#include <ZigbeeLightSensor.h>
#include <ZigbeeLightbulb.h>
#include <ZigbeeTemperatureSensor.h>

ZigbeeTemperatureSensor zigbee_temp_sensor;
ZigbeeHumiditySensor zigbee_humidity_sensor;
ZigbeeLightSensor zigbee_light_sensor;
ZigbeeLightbulb zigbee_bulb;

class ZigbeeHandler {
public:
  static ZigbeeHandler& getInstance() {
    static ZigbeeHandler instance;
    return instance;
  }

  void begin() {
    Zigbee.setVendorName("Bozont Labs Inc.");
    Zigbee.setProductName("auraZigbee");
    Zigbee.setFirmwareVersion(0x00000420);
    Zigbee.begin();
    zigbee_bulb.begin();
    zigbee_bulb.set_onoff(true);
    zigbee_temp_sensor.begin();
    zigbee_humidity_sensor.begin();
    zigbee_light_sensor.begin();
    zigbee_light_sensor.set_min_value_lux(0.0f);
    zigbee_light_sensor.set_max_value_lux(100000.0f);
    zigbee_light_sensor.set_light_sensor_type(ZigbeeLightSensor::LIGHT_SENSOR_TYPE_CMOS);
  }

  void task() {
    this->handle_identify();

    static uint32_t last_action_temp = 0;
    if ((last_action_temp + 5000) < millis()) {
      last_action_temp = millis();
      this->update_sensors();
    }
  }

  void update_sensors() {
    SensorHandler& sensor_handler = SensorHandler::getInstance();

    float temperature = sensor_handler.get_temperature();
    static float temperature_prev = -200.0f;
    if (temperature != temperature_prev) {
      temperature_prev = temperature;
      zigbee_temp_sensor.set_measured_value_celsius(temperature);
    }

    float humidity = sensor_handler.get_humidity();
    static float humidity_prev = -1.0f;
    if (humidity != humidity_prev) {
      humidity_prev = humidity;
      zigbee_humidity_sensor.set_measured_value_percent(humidity);
    }

    float light_level = sensor_handler.get_light_level();
    static float light_level_prev = -1.0f;
    if (light_level != light_level_prev) {
      light_level_prev = light_level;
      zigbee_light_sensor.set_measured_value_lux(light_level);
    }
  }

  bool get_identify_in_progress() {
    return zigbee_bulb.get_identify_in_progress();
  }

private:
  void handle_identify() {
    static bool identification_in_progress_last = false;
    bool identification_in_progress = zigbee_bulb.get_identify_in_progress();
    // Handle the identification enable/disable transitions
    if (identification_in_progress && !identification_in_progress_last) {
      Serial.println("Zigbee identification started");
    } else if (!identification_in_progress && identification_in_progress_last) {
      // Set the LED to the current on/off state once the identification ends
      Serial.println("Zigbee identification ended");
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

  ZigbeeHandler() {}

  ZigbeeHandler(const ZigbeeHandler&) = delete;
  ZigbeeHandler& operator=(const ZigbeeHandler&) = delete;
};
