// ============================================================
// main.cpp — SlaveBox_XiaoS3 (Seeed Studio XIAO ESP32S3)
//
// Build environments (platformio.ini):
//   debug      → DEBUG_MODE=1  Fast loop, full serial, OLED active
//   production → DEBUG_MODE=0  Light sleep, 5-min read interval
//
// Power-save strategy (production):
//   Light sleep preserves BLE connection context and bonding keys.
//   The timer wakeup fires after 5 minutes; the device reads sensors,
//   sends BLE data if connected, then sleeps again.
// ============================================================

#include <Arduino.h>
#include "runSetup.h"
#include "SensorManager.h"
#include "BLEHelper.h"
#include "DisplayHelper.h"

#if !DEBUG_MODE
#include <esp_sleep.h>

// 5-minute sleep interval in microseconds
#define PROD_SLEEP_US  (5ULL * 60ULL * 1000000ULL)
#endif

void setup() {
  runSetup();
  initializeSensors();
  delay(1000);
}

void loop() {
#if DEBUG_MODE
  // ── DEBUG MODE ───────────────────────────────────────────
  // Full-speed loop: verbose sensor output, OLED display cycling.

  std::map<String, std::map<String, float>> sensorData =
      scanAndReadAllSensors(true);

  // Only update OLED when not showing pairing PIN
  if (!bleHelper.isPairing()) {
    if (sensorData.empty()) {
      displayNoSensors();
    } else {
      displaySensorData(sensorData);
    }
  }

  // Send BLE data if connected
  if (bleHelper.isConnected()) {
    bleHelper.sendMap(sensorData);
  }

#else
  // ── PRODUCTION MODE ──────────────────────────────────────
  // Read sensors silently, transmit over BLE if connected,
  // then light-sleep for 5 minutes to save power.

  std::map<String, std::map<String, float>> sensorData =
      scanAndReadAllSensors(false);

  if (bleHelper.isConnected()) {
    bleHelper.sendMap(sensorData);
  }

  // Configure timer wakeup and enter light sleep
  esp_sleep_enable_timer_wakeup(PROD_SLEEP_US);
  esp_light_sleep_start();

#endif // DEBUG_MODE
}
