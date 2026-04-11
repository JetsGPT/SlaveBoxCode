#ifndef DISPLAYHELPER_H
#define DISPLAYHELPER_H

#include <Arduino.h>
#include <map>

/**
 * @brief Display sensor data on the OLED screen by cycling through all metrics
 * 
 * Iterates through the sensor data map and displays each metric one by one
 * with proper formatting (units, decimal places) and a delay between each.
 * 
 * @param sensorData Map of sensor names to their metric key-value pairs
 * @param displayTimeMs How long to show each metric (default 2000ms)
 */
void displaySensorData(const std::map<String, std::map<String, float>>& sensorData, unsigned long displayTimeMs = 2000);

/**
 * @brief Display a "No Sensors" message on the OLED
 */
void displayNoSensors();

#endif // DISPLAYHELPER_H
