#include <Arduino.h>
#include <Wire.h>
#include <esp_system.h>  // For esp_read_mac
#include "SGP30Helper.h"
#include "BLEHelper.h"
#include "ScreenHelper.h"

/**
 * @brief Generate unique device name from ESP32 MAC address
 * Format: RoomSense-XXYYZZ (last 3 bytes of MAC)
 */
String getUniqueDeviceName() {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);  // Use Bluetooth MAC
    char name[24];
    snprintf(name, sizeof(name), "RoomSense-%02X%02X%02X", mac[3], mac[4], mac[5]);
    return String(name);
}

/**
 * @brief Generate unique box ID from ESP32 MAC address
 * Format: box_XXYYZZ
 */
String getUniqueBoxId() {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_BT);
    char boxId[16];
    snprintf(boxId, sizeof(boxId), "box_%02X%02X%02X", mac[3], mac[4], mac[5]);
    return String(boxId);
}

void runSetup() {
    Serial.begin(115200);
    delay(1000); // Give serial monitor time
    Wire.begin(); // Use default I2C pins for Xiao S3

    // Initialize the OLED screen first (before BLE, since pairing uses it)
    if (initScreen()) {
        Serial.println("Screen initialized");
        updateScreen("RoomSense", "Starting...", true);
    } else {
        Serial.println("Screen init failed!");
    }
    delay(500);

    // Initialize BLE with unique device name and box ID (from MAC address)
    String deviceName = getUniqueDeviceName();
    String boxId = getUniqueBoxId();
    Serial.print("Device Name: "); Serial.println(deviceName);
    Serial.print("Box ID: "); Serial.println(boxId);
    bleHelper.begin(deviceName, boxId);

    // Show ready message on screen
    updateScreen("RoomSense", "Ready!", true);

    Serial.println("Setup completed");
}

