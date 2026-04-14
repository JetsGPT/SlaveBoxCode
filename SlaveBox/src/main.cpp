// ============================================================
// main.cpp — SlaveBox (ESP32 WROOM)
//
// Build environments (platformio.ini):
//   debug      → DEBUG_MODE=1  Fast loop, full serial, OLED active
//   production → DEBUG_MODE=0  Light sleep, 5-min read interval, OLED off
//
// Power-save strategy (production):
//   Light sleep is used instead of deep sleep so the BLE connection
//   context and bonding keys are preserved. The CPU wakes automatically
//   after PROD_SLEEP_US microseconds via the esp_timer wakeup source,
//   reads all sensors, sends BLE data (if connected), then sleeps again.
// ============================================================

#include <Arduino.h>
#include "runSetup.h"
#include "SensorManager.h"
#include "BLEHelper.h"
#include "DisplayHelper.h"
#include "ButtonHandler.h"
#include "ScreenPowerManager.h"

#if DEBUG_MODE
// ── Debug includes ──────────────────────────────────────────
#else
// ── Production includes ─────────────────────────────────────
#include <esp_sleep.h>

// 5-minute sleep interval in microseconds
#define PROD_SLEEP_US  (5ULL * 60ULL * 1000000ULL)
#endif

// Button on GPIO 4 (connect between GPIO4 and GND)
ButtonHandler button(4);

void setup() {
  runSetup();
  initializeSensors();

  // Initialize button handler
  button.begin();

#if DEBUG_MODE
  // Initialize screen power manager (starts with screen OFF)
  screenPowerManager.begin();
#endif

  delay(1000);
}

void loop() {
#if DEBUG_MODE
  // ── DEBUG MODE ───────────────────────────────────────────
  // Full-speed loop: buttons, screen power, OLED display cycling,
  // verbose sensor output.

  // Update button state (must be called every loop)
  button.update();

  // Update screen power manager (handles auto-timeout)
  screenPowerManager.update();

  // Handle button presses
  if (button.wasLongPressed()) {
    screenPowerManager.toggleAlwaysOn(); // Long press: toggle always-on
  } else if (button.wasPressed()) {
    screenPowerManager.wake();           // Short press: wake + reset timer
  }

  // Read all sensors with verbose serial output
  std::map<String, std::map<String, float>> sensorData =
      scanAndReadAllSensors(true);

  // Update OLED when screen is on and not showing pairing PIN
  if (screenPowerManager.isScreenOn() && !bleHelper.isPairing()) {
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

  // Read sensors (silent — no verbose serial output)
  std::map<String, std::map<String, float>> sensorData =
      scanAndReadAllSensors(false);

  // Transmit over BLE if a client is connected
  if (bleHelper.isConnected()) {
    bleHelper.sendMap(sensorData);
  }

  // Configure light sleep: wake automatically after PROD_SLEEP_US
  esp_sleep_enable_timer_wakeup(PROD_SLEEP_US);

  // Enter light sleep — BLE stack, RAM, and bonding keys are preserved.
  // CPU resumes here on wake.
  esp_light_sleep_start();

#endif // DEBUG_MODE
}
