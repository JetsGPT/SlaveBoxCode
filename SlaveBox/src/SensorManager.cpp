#include "SensorManager.h"
#include "Bme280Helper.h"
#include "Sgp30Helper.h"
#include "Bh1750Helper.h"
#include <Wire.h>

// --- Helper wrapper functions ---
static std::map<String, float> readBME280(uint8_t addr, bool debug) {
  return getValues_BME280(addr, debug);
}

static std::map<String, float> readSGP30Wrapper(uint8_t addr, bool debug) {
  return getValues_SGP30(debug);
}

static std::map<String, float> readBH1750(uint8_t addr, bool debug) {
  return getValues_BH1750(addr, debug);
}

// --- List of known sensors with their read functions ---
static SensorInfo sensors[] = {
  {0x77, "BME280", readBME280},
  {0x58, "SGP30", readSGP30Wrapper},
  {0x23, "BH1750", readBH1750}
};

static const int numSensors = sizeof(sensors) / sizeof(sensors[0]);

// --- Initialize sensors ---
void initializeSensors() {
  Wire.begin();
  delay(100);
}

// --- Get sensor info by address ---
SensorInfo* getSensorByAddress(uint8_t addr) {
  for (int i = 0; i < numSensors; i++) {
    if (sensors[i].addr == addr) {
      return &sensors[i];
    }
  }
  return nullptr;
}

// --- Scan and read all sensors ---
std::map<String, std::map<String, float>> scanAndReadAllSensors(bool debug) {
  std::map<String, std::map<String, float>> allSensorData;
  
  if (debug) {
    Serial.println("\n=== Scanning I2C bus ===");
  }
  
  for (int i = 0; i < numSensors; i++) {
    uint8_t addr = sensors[i].addr;
    
    Wire.beginTransmission(addr);
    uint8_t error = Wire.endTransmission();

    if (error == 0) {  // Sensor found
      if (debug) {
        Serial.print("\n✓ Found ");
        Serial.print(sensors[i].name);
        Serial.print(" at 0x");
        if (addr < 16) Serial.print('0');
        Serial.println(addr, HEX);
      }

      // Read sensor values
      if (sensors[i].readFunction != nullptr) {
        std::map<String, float> values = sensors[i].readFunction(addr, false);
        
        // Store in the main dictionary with sensor name as key
        allSensorData[sensors[i].name] = values;
        
        if (debug) {
          Serial.println("  Readings:");
          for (auto const& pair : values) {
            Serial.print("    ");
            Serial.print(pair.first);
            Serial.print(": ");
            Serial.println(pair.second);
          }
        }
      }
    } else if (debug) {
      Serial.print("✗ ");
      Serial.print(sensors[i].name);
      Serial.print(" not found at 0x");
      if (addr < 16) Serial.print('0');
      Serial.println(addr, HEX);
    }
  }
  
  if (debug) {
    Serial.println("\n=== Scan complete ===\n");
  }
  
  return allSensorData;
}
