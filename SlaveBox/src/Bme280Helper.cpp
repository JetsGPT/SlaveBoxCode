#include <Wire.h>
#include <Adafruit_BME280.h>
#include <map>

Adafruit_BME280 bme; // I2C

//return Name - Value in a Map
// addr - Sensors address
std::map<String, float> getValues_BME280(uint8_t addr, boolean debug) {

   Wire.begin();
  if (!bme.begin(addr)) {  // Check the sensor's I2C address
    if(debug)
    {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
    }
    
    while (1);
  }
  std::map<String, float> values;
  values["temperature"] = bme.readTemperature(); // Temperature in °C
  values["humidity"] = bme.readHumidity(); // Humidity in %
  values["pressure"] = bme.readPressure() / 100.0F; // Pressure in hPa

  //Serial.print(bme.readTemperature()); // Temperature in °C
  //Serial.print(bme.readHumidity()); // Humidity in %
  //Serial.print(bme.readPressure() / 100.0F); // Pressure in hPa

  return values;
}