#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>
#include <map>

// --- Sensor configuration structure (mirrors real SensorManager.h) ---
struct SensorInfo {
  uint8_t     addr;
  const char* name;
  // Read function pointer (addr kept for API parity with real code)
  std::map<String, float> (*readFunction)(uint8_t, bool);
};

// Initialize the mock sensor manager (no-op in mock — emulates Wire.begin())
void initializeSensors();

// Scan and read all (mock) sensors, returning the same nested-map format
// as the real scanAndReadAllSensors()
std::map<String, std::map<String, float>> scanAndReadAllSensors(bool debug = true);

// Get sensor info by I2C address (mock version)
SensorInfo* getSensorByAddress(uint8_t addr);

#endif // SENSORMANAGER_H
