#include <Arduino.h>
#include "runSetup.h"
#include "SensorManager.h"
#include "BLEHelper.h"
#include "DisplayHelper.h"
#include "ButtonHandler.h"
#include "ScreenPowerManager.h"

// Button on GPIO 4 (connect button between GPIO4 and GND)
ButtonHandler button(4);

void setup() {
  runSetup();
  initializeSensors();
  
  // Initialize button handler
  button.begin();
  
  // Initialize screen power manager (starts with screen OFF)
  screenPowerManager.begin();
  
  delay(1000);
}

void loop() {
  // Update button state (must be called every loop)
  button.update();
  
  // Update screen power manager (handles auto-timeout)
  screenPowerManager.update();
  
  // Handle button presses
  if (button.wasLongPressed()) {
    // Long press: toggle always-on mode
    screenPowerManager.toggleAlwaysOn();
  } else if (button.wasPressed()) {
    // Short press: wake screen and reset timeout
    screenPowerManager.wake();
  }
  
  // Get sensor data
  std::map<String, std::map<String, float>> sensorData = scanAndReadAllSensors(true);

  // Only update OLED when screen is on and not showing pairing PIN
  if (screenPowerManager.isScreenOn() && !bleHelper.isPairing()) {
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

