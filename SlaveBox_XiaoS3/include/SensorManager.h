#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>
#include <map>

// --- Sensor configuration structure ---
struct SensorInfo {
  uint8_t addr;
  const char* name;
  std::map<String, float> (*readFunction)(uint8_t, bool);
};

void initializeSensors();
std::map<String, std::map<String, float>> scanAndReadAllSensors(bool debug = true);
SensorInfo* getSensorByAddress(uint8_t addr);

#endif
