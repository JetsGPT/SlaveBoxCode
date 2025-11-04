#include <Arduino.h>
#include <Wire.h>
#include "SGP30Helper.h"
#include "BLEHelper.h"

void runSetup() {
    Serial.begin(115200);
    delay(1000); // Give serial monitor time
    Wire.begin(21, 22); // ESP32 I2C pins

    // Initialize BLE. Device name and box id can be changed here.
    bleHelper.begin("TempSensor01", "box_room_001");

    Serial.println("Setup completed");
}
