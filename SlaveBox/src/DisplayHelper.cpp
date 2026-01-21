#include "DisplayHelper.h"
#include "ScreenHelper.h"
#include "BLEHelper.h"
#include "ScreenPowerManager.h"
#include "ButtonHandler.h"

// External button handler from main.cpp
extern ButtonHandler button;

/**
 * @brief Format a sensor value with the appropriate precision and unit
 * @param metricName The name of the metric (e.g., "temperature", "humidity")
 * @param value The raw float value from the sensor
 * @return Formatted string suitable for OLED display (e.g., "24.5 C")
 */
static String formatSensorValue(const String& metricName, float value) {
  String lowerMetric = metricName;
  lowerMetric.toLowerCase();

  if (lowerMetric == "temperature") {
    return String(value, 1) + " C";
  } else if (lowerMetric == "humidity") {
    return String(value, 1) + " %";
  } else if (lowerMetric == "pressure") {
    return String(value, 0) + " hPa";
  } else if (lowerMetric == "light" || lowerMetric == "lux") {
    return String(value, 0) + " lx";
  } else if (lowerMetric == "eco2" || lowerMetric == "co2") {
    return String(value, 0) + " ppm";
  } else if (lowerMetric == "tvoc") {
    return String(value, 0) + " ppb";
  }
  
  // Default: 1 decimal place, no unit
  return String(value, 1);
}

/**
 * @brief Create a readable header from metric name
 * @param metricName The name of the metric (e.g., "temperature")
 * @return A human-readable header string with capitalized first letter
 */
static String formatHeader(const String& metricName) {
  String header = metricName;
  if (header.length() > 0) {
    header[0] = toupper(header[0]);
  }
  return header;
}

/**
 * @brief Check if display should exit and handle button input during wait
 * @return true if should exit (pairing, screen off, or button pressed)
 */
static bool shouldExitDisplay() {
  button.update();
  
  // Handle button presses during display
  if (button.wasLongPressed()) {
    screenPowerManager.toggleAlwaysOn();
    return true;  // Exit to let main loop handle new state
  }
  if (button.wasPressed()) {
    screenPowerManager.wake();
    // Don't exit - just reset the timeout
  }
  
  return bleHelper.isPairing() || !screenPowerManager.isScreenOn();
}

void displaySensorData(const std::map<String, std::map<String, float>>& sensorData, unsigned long displayTimeMs) {
  for (const auto& sensorEntry : sensorData) {
    if (shouldExitDisplay()) return;
    
    const String& sensorName = sensorEntry.first;
    const std::map<String, float>& metrics = sensorEntry.second;
    
    for (const auto& metricEntry : metrics) {
      if (shouldExitDisplay()) return;
      
      const String& metricName = metricEntry.first;
      float value = metricEntry.second;
      
      String header = formatHeader(metricName);
      String formattedValue = formatSensorValue(metricName, value);
      
      updateScreen(header, formattedValue, true);
      
      // Wait with frequent checks for button input
      unsigned long elapsed = 0;
      while (elapsed < displayTimeMs) {
        if (shouldExitDisplay()) return;
        delay(50);  // Shorter delay for more responsive button handling
        elapsed += 50;
      }
    }
  }
}

void displayNoSensors() {
  if (shouldExitDisplay()) return;
  updateScreen("Status", "No Sensors", true);
  
  unsigned long elapsed = 0;
  while (elapsed < 2000) {
    if (shouldExitDisplay()) return;
    delay(50);
    elapsed += 50;
  }
}

