// ============================================================
// MockSensorManager.cpp
// Realistic simulated sensor data for the XIAO ESP32-C3 mock.
//
// Approach:
//   • Sinusoidal drift on slow time bases gives organic variation
//   • Random noise added on each reading (esp_random() seeded once)
//   • SGP30 warm-up replicated: eCO2 starts at 400 ppm and drifts
//   • All values clamped to real-world physical limits
// ============================================================

#include "MockSensorManager.h"
#include <Arduino.h>
#include <cmath>

// ---- Internal helpers ----------------------------------------

// Clamp a float between lo and hi
static float clamp(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// Deterministic small gaussian-like noise using millis phase
static float noise(float amplitude) {
    // Combine two fast sinusoids at co-prime periods to approximate white noise
    float t = (float)millis() / 1000.0f;
    return amplitude * (sin(t * 13.7f) + sin(t * 19.3f)) * 0.5f;
}

// ---- BME280 mock --------------------------------------------
// Baseline resembling a typical room in Europe in spring:
//   temperature  ~22 °C, humidity ~55 %, pressure ~1013 hPa

std::map<String, float> mockGetValues_BME280(bool debug) {
    float t = (float)millis() / 1000.0f;

    // Slow drift (period ~2 minutes) + fast noise
    float temperature = 22.0f
                      + 1.5f * sin(t / 120.0f)   // slow oscillation ±1.5 °C
                      + noise(0.2f);              // high-frequency noise ±0.2 °C
    temperature = clamp(temperature, -40.0f, 85.0f);

    float humidity = 55.0f
                   + 5.0f  * sin(t / 90.0f + 1.0f)  // drift ±5 %
                   + noise(0.5f);                      // noise ±0.5 %
    humidity = clamp(humidity, 0.0f, 100.0f);

    // Pressure changes very slowly (~1–2 hPa over minutes)
    float pressure = 1013.25f
                   + 2.0f * sin(t / 300.0f)   // drift ±2 hPa
                   + noise(0.1f);              // noise ±0.1 hPa
    pressure = clamp(pressure, 300.0f, 1100.0f);

    std::map<String, float> values;
    values["temperature"] = temperature;
    values["humidity"]    = humidity;
    values["pressure"]    = pressure;

    if (debug) {
        Serial.printf("[MOCK BME280] temp=%.2f°C  hum=%.1f%%  pres=%.2f hPa\n",
                      temperature, humidity, pressure);
    }
    return values;
}

// ---- SGP30 mock --------------------------------------------
// SGP30 needs ~15 s warm-up before values stabilise.
// eCO2 floor: 400 ppm (ambient outdoor CO2), rises in a "room" scenario.

static bool sgp30WarmUpDone = false;
static unsigned long sgp30StartMs = 0;

std::map<String, float> mockGetValues_SGP30(bool debug) {
    if (sgp30StartMs == 0) sgp30StartMs = millis();

    std::map<String, float> values;

    unsigned long elapsedMs = millis() - sgp30StartMs;

    // Replicates the real warm-up period: during the first 15 s, readings
    // are 400 ppm / 0 ppb (same as real hardware reset state)
    if (elapsedMs < 15000UL) {
        values["eCO2"] = 400.0f;
        values["TVOC"] = 0.0f;
        if (debug) {
            Serial.printf("[MOCK SGP30] Warming up... (%lu s remaining)\n",
                          (15000UL - elapsedMs) / 1000UL);
        }
        return values;
    }

    float t = (float)elapsedMs / 1000.0f;

    // After warm-up, simulate an occupied room:
    //   eCO2: 400–900 ppm, TVOC: 0–150 ppb
    float eCO2 = 600.0f
               + 200.0f * sin(t / 180.0f)  // slow drift ±200 ppm
               + noise(15.0f);              // sensor noise ±15 ppm
    eCO2 = clamp(eCO2, 400.0f, 8192.0f);

    float tvoc = 40.0f
               + 30.0f * sin(t / 120.0f + 0.5f)
               + noise(5.0f);
    tvoc = clamp(tvoc, 0.0f, 60000.0f);

    values["eCO2"] = eCO2;
    values["TVOC"] = tvoc;

    if (debug) {
        Serial.printf("[MOCK SGP30]  eCO2=%.0f ppm  TVOC=%.0f ppb\n", eCO2, tvoc);
    }
    return values;
}

// ---- BH1750 mock -------------------------------------------
// Simulate daylight level that changes as if a cloud passes every few minutes

std::map<String, float> mockGetValues_BH1750(bool debug) {
    float t = (float)millis() / 1000.0f;

    // Indoor daylight: ~300 lux base, clouds reduce it periodically
    float lux = 350.0f
              + 120.0f * sin(t / 150.0f)     // slow cloud drift ±120 lux
              + 20.0f  * sin(t / 7.0f)       // fast ripple (fluorescent flicker) ±20
              + noise(5.0f);                   // noise
    lux = clamp(lux, 0.0f, 65535.0f);

    std::map<String, float> values;
    values["light"] = lux;

    if (debug) {
        Serial.printf("[MOCK BH1750] light=%.1f lux\n", lux);
    }
    return values;
}
