#include <Arduino.h>
#include <Wire.h>
#include "SGP30Helper.h"
#include "BLEHelper.h"
#include "ScreenHelper.h"

void runSetup() {
    Serial.begin(115200);
    delay(1000); // Give serial monitor time
    Wire.begin(21, 22); // ESP32 I2C pins

    // Initialize the OLED screen first (before BLE, since pairing uses it)
    if (initScreen()) {
        Serial.println("Screen initialized");
        updateScreen("RoomSense", "Starting...", true);
    } else {
        Serial.println("Screen init failed!");
    }
    delay(500);

    // Initialize BLE. Device name and box id can be changed here.
    bleHelper.begin("RoomSense-Sensor-01", "box_room_001");

    // Show ready message on screen
    updateScreen("RoomSense", "Ready!", true);

    Serial.println("Setup completed");
}
