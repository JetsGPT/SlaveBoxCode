#ifndef DISPLAYHELPER_H
#define DISPLAYHELPER_H

#include <Arduino.h>
#include <map>

// Cycle through all sensor metrics, showing each on the OLED for displayTimeMs
// Defaults to 2 seconds per metric, matching the real implementation
void displaySensorData(const std::map<String, std::map<String, float>>& sensorData,
                       unsigned long displayTimeMs = 2000);

// Show a "No Sensors" placeholder on the OLED
void displayNoSensors();

#endif // DISPLAYHELPER_H
