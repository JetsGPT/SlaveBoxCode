// ============================================================
// SensorManager.cpp  (mock version)
// Mirrors the real SensorManager API exactly.
// Instead of performing I2C probes the mock always "finds" all three
// sensors and delegates to MockSensorManager for readings.
// ============================================================

#include "SensorManager.h"
#include "MockSensorManager.h"
#include <Arduino.h>

// ---- Wrapper shims (match real SensorManager function-pointer signature) ---
static std::map<String, float> readBME280Mock(uint8_t /*addr*/, bool debug) {
    return mockGetValues_BME280(debug);
}

static std::map<String, float> readSGP30Mock(uint8_t /*addr*/, bool debug) {
    return mockGetValues_SGP30(debug);
}

static std::map<String, float> readBH1750Mock(uint8_t /*addr*/, bool debug) {
    return mockGetValues_BH1750(debug);
}

// ---- Known (mock) sensor table --------------------------------
static SensorInfo sensors[] = {
    {MOCK_ADDR_BME280, MOCK_SENSOR_BME280, readBME280Mock},
    {MOCK_ADDR_SGP30,  MOCK_SENSOR_SGP30,  readSGP30Mock},
    {MOCK_ADDR_BH1750, MOCK_SENSOR_BH1750, readBH1750Mock},
};

static const int numSensors = sizeof(sensors) / sizeof(sensors[0]);

// ---- Public API -----------------------------------------------

void initializeSensors() {
    // In the real code this calls Wire.begin().
    // In mock mode the OLED already called Wire.begin() in runSetup, so we
    // just print a friendly message to confirm mock init.
    Serial.println("[MOCK] SensorManager initialized (no I2C scan performed)");
    delay(100);
}

SensorInfo* getSensorByAddress(uint8_t addr) {
    for (int i = 0; i < numSensors; i++) {
        if (sensors[i].addr == addr) return &sensors[i];
    }
    return nullptr;
}

std::map<String, std::map<String, float>> scanAndReadAllSensors(bool debug) {
    std::map<String, std::map<String, float>> allSensorData;

    if (debug) {
        Serial.println("\n=== [MOCK] Scanning I2C bus ===");
    }

    for (int i = 0; i < numSensors; i++) {
        // In mock mode every sensor is always "found" — emulating the
        // real I2C ACK (error == 0) path in the original SensorManager.
        if (debug) {
            Serial.print("\n✓ [MOCK] Found ");
            Serial.print(sensors[i].name);
            Serial.print(" at 0x");
            if (sensors[i].addr < 16) Serial.print('0');
            Serial.println(sensors[i].addr, HEX);
        }

        if (sensors[i].readFunction != nullptr) {
            std::map<String, float> values = sensors[i].readFunction(sensors[i].addr, false);
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
    }

    if (debug) {
        Serial.println("\n=== [MOCK] Scan complete ===\n");
    }

    return allSensorData;
}
