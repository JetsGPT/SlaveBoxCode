#include <Arduino.h>
#include "runSetup.h"
#include "SensorManager.h"
#include "BLEHelper.h"
#include "DisplayHelper.h"

void setup() {
  runSetup();
  initializeSensors();
  
  delay(1000);
}

void loop() {
  // Get sensor data
  std::map<String, std::map<String, float>> sensorData = scanAndReadAllSensors(true);

  // Only update OLED when not showing pairing PIN
  if (!bleHelper.isPairing()) {
    if (sensorData.empty()) {
      displayNoSensors();
    } else {
      displaySensorData(sensorData);
    }
  }

  // Send sensor data over BLE (if connected)
  if (bleHelper.isConnected()) {
    bleHelper.sendMap(sensorData);
  }
}

