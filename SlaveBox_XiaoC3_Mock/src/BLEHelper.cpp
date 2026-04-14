// ============================================================
// BLEHelper.cpp  (mock stub)
// Mirrors the real BLEHelper API.
// sendMap() serialises data to JSON and prints to Serial in exactly
// the same format as the real BLE notify path, so the output can be
// copy-pasted into a Python bridge for quick testing.
// ============================================================

#include "BLEHelper.h"
#include <ArduinoJson.h>
#include <Arduino.h>

BLEHelper bleHelper;  // global instance (mirrors real code)

void BLEHelper::begin(const String& deviceName, const String& boxId) {
    Serial.println("========================================");
    Serial.print("[MOCK BLE] Device Name : "); Serial.println(deviceName);
    Serial.print("[MOCK BLE] Box ID      : "); Serial.println(boxId);
    Serial.println("[MOCK BLE] BLE stack NOT started (mock mode)");
    Serial.println("[MOCK BLE] sendMap() will log JSON to Serial instead");
    Serial.println("========================================");
}

void BLEHelper::sendMap(const std::map<String, std::map<String, float>>& data) {
    // Mirrors the real BLEHelper::sendMap() output format so the Python bridge
    // can validate JSON payloads without real hardware.
    Serial.println("--- [MOCK BLE] Sending sensor map (Serial only) ---");

    for (const auto& sensorPair : data) {
        const String& sensorName = sensorPair.first;
        const std::map<String, float>& metrics = sensorPair.second;

        for (const auto& metricPair : metrics) {
            const String& metricName = metricPair.first;
            float value = metricPair.second;

            // Print readable line (same format as real code)
            Serial.print(sensorName); Serial.print("/");
            Serial.print(metricName); Serial.print(": ");
            Serial.println(value, 4);

            // Serialize to JSON (identical structure to real BLE notify payload)
            JsonDocument doc;
            doc["type"]  = metricName;
            doc["value"] = value;

            String jsonString;
            serializeJson(doc, jsonString);
            Serial.print("  -> [MOCK] JSON: ");
            Serial.println(jsonString);

            // Brief delay to pace output (mirrors the real 20 ms + 60 ms delays)
            delay(20);
        }
    }
}
