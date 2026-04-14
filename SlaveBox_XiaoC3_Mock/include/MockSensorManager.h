#ifndef MOCK_SENSOR_MANAGER_H
#define MOCK_SENSOR_MANAGER_H

#include <Arduino.h>
#include <map>

// ============================================================
// MockSensorManager — generates realistic simulated sensor data
// without any physical I2C devices attached.
//
// The mock uses millis()-based sinusoidal drift and small random
// noise to produce readings that evolve naturally over time,
// closely matching what real sensors would produce in a room.
// ============================================================

// Sensor names (matching the real code exactly)
#define MOCK_SENSOR_BME280  "BME280"
#define MOCK_SENSOR_SGP30   "SGP30"
#define MOCK_SENSOR_BH1750  "BH1750"

// Emulated I2C addresses (kept for structural parity with real code)
#define MOCK_ADDR_BME280    0x77
#define MOCK_ADDR_SGP30     0x58
#define MOCK_ADDR_BH1750    0x23

// Returns mock BME280 data: temperature (°C), humidity (%), pressure (hPa)
std::map<String, float> mockGetValues_BME280(bool debug = false);

// Returns mock SGP30 data: eCO2 (ppm), TVOC (ppb)
std::map<String, float> mockGetValues_SGP30(bool debug = false);

// Returns mock BH1750 data: light (lux)
std::map<String, float> mockGetValues_BH1750(bool debug = false);

#endif // MOCK_SENSOR_MANAGER_H
