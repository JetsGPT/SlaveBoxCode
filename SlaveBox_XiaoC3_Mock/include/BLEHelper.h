#ifndef BLEHELPER_H
#define BLEHELPER_H

#include <Arduino.h>
#include <map>

// ============================================================
// BLEHelper — mock stub for the ESP32-C3 Xiao
//
// On the C3, actual BLE stack support (esp_nimble / bluedroid)
// is available but wiring it up fully is outside the scope of
// this mock. This stub:
//   • mirrors the exact public API of the real BLEHelper
//   • serialises sensor data to JSON and prints it to Serial
//     in the same format that the real BLE NOTIFY does
//   • isConnected() returns false so the mock loop skips real BLE
//
// To enable real BLE on the C3, replace this file with the
// actual BLEHelper implementation and add the BLE lib_deps.
// ============================================================
class BLEHelper {
public:
    BLEHelper() {}

    void begin(const String& deviceName, const String& boxId);
    void sendMap(const std::map<String, std::map<String, float>>& data);

    // Always returns false in mock — no active BLE connection
    bool isConnected()  { return false; }
    // Always returns false — no pairing flow
    bool isPairing()    { return false; }
};

extern BLEHelper bleHelper;

#endif // BLEHELPER_H
