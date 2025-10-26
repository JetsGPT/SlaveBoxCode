#include <Arduino.h>
#include <Wire.h>
#include "SGP30Helper.h"

void runSetup() {
    Serial.begin(115200);
    delay(1000); // Give serial monitor time
    Wire.begin(21, 22); // ESP32 I2C pins

    Serial.println("Setup completed");
}
