// ============================================================
// runSetup.cpp  (mock version for XIAO ESP32-C3)
// Mirrors the real runSetup.cpp but:
//   - uses the Bluetooth MAC via esp_read_mac (same as real code)
//   - calls BLEHelper stub (no actual BLE stack started)
//   - initialises the OLED display (real SSD1306 on I2C)
// ============================================================

#include <Arduino.h>
#include <Wire.h>
#include <esp_system.h>  // esp_read_mac
#include "BLEHelper.h"
#include "ScreenHelper.h"
#include "runSetup.h"

// Generate unique device name from ESP32-C3 Bluetooth MAC
// Format: RoomSense-XXYYZZ  (same convention as real firmware)
static String getUniqueDeviceName() {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    char name[24];
    snprintf(name, sizeof(name), "RoomSense-%02X%02X%02X", mac[3], mac[4], mac[5]);
    return String(name);
}

// Generate unique box ID from ESP32-C3 Bluetooth MAC
// Format: box_XXYYZZ
static String getUniqueBoxId() {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    char boxId[16];
    snprintf(boxId, sizeof(boxId), "box_%02X%02X%02X", mac[3], mac[4], mac[5]);
    return String(boxId);
}

void runSetup() {
    Serial.begin(115200);
    delay(1000);  // Let the serial monitor attach

    Serial.println("========================================");
    Serial.println("  RoomSense MOCK — XIAO ESP32-C3       ");
    Serial.println("  No physical sensors required         ");
    Serial.println("========================================");

    // Initialise I2C on the C3 default pins (SDA=GPIO6, SCL=GPIO7)
    Wire.begin();

    // Initialise OLED display (real hardware — also supported on C3)
    if (initScreen()) {
        Serial.println("OLED screen initialized");
        updateScreen("RoomSense", "MOCK Mode", true);
    } else {
        Serial.println("OLED screen not found — running headless");
    }
    delay(500);

    // Print unique device identifiers (derived from hardware MAC)
    String deviceName = getUniqueDeviceName();
    String boxId      = getUniqueBoxId();
    Serial.print("Device Name : "); Serial.println(deviceName);
    Serial.print("Box ID      : "); Serial.println(boxId);

    // Initialise BLE stub (logs config to Serial, no BLE stack)
    bleHelper.begin(deviceName, boxId);

    // Show ready banner on OLED
    updateScreen("RoomSense", "Ready!", true);

    Serial.println("[MOCK] Setup completed ✅");
}
