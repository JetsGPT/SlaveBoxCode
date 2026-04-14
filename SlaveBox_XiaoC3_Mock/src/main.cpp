// ============================================================
// main.cpp  (mock version for XIAO ESP32-C3)
//
// The main loop is intentionally identical to the real S3 firmware.
// Only the underlying implementations differ (mock vs hardware):
//
//   • scanAndReadAllSensors() → MockSensorManager data
//   • bleHelper.isConnected() → always false (BLE stub)
//   • bleHelper.isPairing()   → always false (BLE stub)
//   • displaySensorData()     → real SSD1306 output (if wired)
//
// This makes it trivial to swap in the real implementations later
// by replacing the mock files without touching main.cpp at all.
// ============================================================

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
    // Get (mock) sensor data — same map format as real firmware
    std::map<String, std::map<String, float>> sensorData =
        scanAndReadAllSensors(true);

    // Update OLED when not showing pairing PIN (always true in mock)
    if (!bleHelper.isPairing()) {
        if (sensorData.empty()) {
            displayNoSensors();
        } else {
            displaySensorData(sensorData);
        }
    }

    // Send data over BLE if connected (skipped in mock — never connected)
    if (bleHelper.isConnected()) {
        bleHelper.sendMap(sensorData);
    }
}
