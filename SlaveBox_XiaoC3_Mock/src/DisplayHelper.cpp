// ============================================================
// DisplayHelper.cpp  (mock version)
// Direct port of the real DisplayHelper — formats sensor values
// for the OLED display. isPairing() always returns false in the
// mock, so shouldExitDisplay() never fires.
// ============================================================

#include "DisplayHelper.h"
#include "ScreenHelper.h"
#include "BLEHelper.h"

// Format a sensor value with the appropriate precision and unit
static String formatSensorValue(const String& metricName, float value) {
    String m = metricName;
    m.toLowerCase();

    if (m == "temperature") return String(value, 1) + " C";
    if (m == "humidity")    return String(value, 1) + " %";
    if (m == "pressure")    return String(value, 0) + " hPa";
    if (m == "light" || m == "lux") return String(value, 0) + " lx";
    if (m == "eco2" || m == "co2")  return String(value, 0) + " ppm";
    if (m == "tvoc")        return String(value, 0) + " ppb";
    return String(value, 1);
}

// Capitalise first letter for header display
static String formatHeader(const String& metricName) {
    String h = metricName;
    if (h.length() > 0) h[0] = toupper(h[0]);
    return h;
}

static bool shouldExitDisplay() {
    return bleHelper.isPairing();  // always false in mock
}

void displaySensorData(const std::map<String, std::map<String, float>>& sensorData,
                       unsigned long displayTimeMs) {
    for (const auto& sensorEntry : sensorData) {
        if (shouldExitDisplay()) return;
        const std::map<String, float>& metrics = sensorEntry.second;

        for (const auto& metricEntry : metrics) {
            if (shouldExitDisplay()) return;

            const String& metricName = metricEntry.first;
            float value = metricEntry.second;

            updateScreen(formatHeader(metricName),
                         formatSensorValue(metricName, value),
                         true);

            unsigned long elapsed = 0;
            while (elapsed < displayTimeMs) {
                if (shouldExitDisplay()) return;
                delay(50);
                elapsed += 50;
            }
        }
    }
}

void displayNoSensors() {
    if (shouldExitDisplay()) return;
    updateScreen("Status", "No Sensors", true);

    unsigned long elapsed = 0;
    while (elapsed < 2000) {
        if (shouldExitDisplay()) return;
        delay(50);
        elapsed += 50;
    }
}
