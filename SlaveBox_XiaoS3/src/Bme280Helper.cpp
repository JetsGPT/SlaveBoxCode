#include <Wire.h>
#include <Adafruit_BME280.h>
#include <map>
#include <cmath>

Adafruit_BME280 bme; // I2C
static bool bme280_initialized = false;

// Validation constants for realistic environmental ranges
#define BME280_TEMP_MIN -40.0f
#define BME280_TEMP_MAX 85.0f
#define BME280_HUMIDITY_MIN 0.0f
#define BME280_HUMIDITY_MAX 100.0f
#define BME280_PRESSURE_MIN 300.0f
#define BME280_PRESSURE_MAX 1100.0f
#define SENSOR_ERROR_VALUE -999.0f

//return Name - Value in a Map
// addr - Sensors address
std::map<String, float> getValues_BME280(uint8_t addr, boolean debug) {
  std::map<String, float> values;

  // Initialize only once, with graceful failure handling
  if (!bme280_initialized) {
    Wire.begin();
    if (!bme.begin(addr)) {
      if(debug) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
      }
      // Return error values instead of hanging - allows device to continue
      values["temperature"] = SENSOR_ERROR_VALUE;
      values["humidity"] = SENSOR_ERROR_VALUE;
      values["pressure"] = SENSOR_ERROR_VALUE;
      return values;
    }
    bme280_initialized = true;
    if(debug) Serial.println("BME280 sensor initialized ✅");
  }

  // Read and validate temperature
  float temp = bme.readTemperature();
  if (isnan(temp) || temp < BME280_TEMP_MIN || temp > BME280_TEMP_MAX) {
    values["temperature"] = SENSOR_ERROR_VALUE;
    if(debug) Serial.println("BME280: Invalid temperature reading");
  } else {
    values["temperature"] = temp;
  }

  // Read and validate humidity
  float humidity = bme.readHumidity();
  if (isnan(humidity) || humidity < BME280_HUMIDITY_MIN || humidity > BME280_HUMIDITY_MAX) {
    values["humidity"] = SENSOR_ERROR_VALUE;
    if(debug) Serial.println("BME280: Invalid humidity reading");
  } else {
    values["humidity"] = humidity;
  }

  // Read and validate pressure
  float pressure = bme.readPressure() / 100.0F;
  if (isnan(pressure) || pressure < BME280_PRESSURE_MIN || pressure > BME280_PRESSURE_MAX) {
    values["pressure"] = SENSOR_ERROR_VALUE;
    if(debug) Serial.println("BME280: Invalid pressure reading");
  } else {
    values["pressure"] = pressure;
  }

  return values;
}