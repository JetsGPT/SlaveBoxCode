#include <Arduino.h>
#include "runSetup.h"
#include "SensorManager.h"
#include "BLEHelper.h"

void setup() {
  runSetup();
  initializeSensors();
  delay(1000);
}

void loop() {
  // Get all sensor data as a structured dictionary
  std::map<String, std::map<String, float>> sensorData = scanAndReadAllSensors(true);
  
  // Example:
  //if (sensorData.count("BME280") > 0) {
  //  float temp = sensorData["BME280"]["temperature"];
  //  float humidity = sensorData["BME280"]["humidity"];
  //  Serial.print("Temperature: "); Serial.println(temp);
  //  Serial.print("Humidity: "); Serial.println(humidity);
  //}

  // Send sensor data over BLE (if connected)
  if (bleHelper.isConnected()) {
    bleHelper.sendMap(sensorData);
  }

  delay(5000);
}
