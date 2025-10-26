#include "Bh1750Helper.h"
#include <Wire.h>
#include <BH1750.h>

static BH1750 lightMeter;
static bool initialized = false;

// Return Name - Value in a Map
// addr - Sensor's address (default 0x23)
std::map<String, float> getValues_BH1750(uint8_t addr, bool debug) {
    if (!initialized) {
        if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, addr, &Wire)) {
            if (debug) {
                Serial.println("Could not find a valid BH1750 sensor, check wiring!");
            }
            while (1);
        }
        if (debug) Serial.println("BH1750 sensor initialized ✅");
        initialized = true;
    }

    std::map<String, float> values;
    
    float lux = lightMeter.readLightLevel();
    values["light"] = lux; // Light level in lux

    if (debug) {
        Serial.print("Light: ");
        Serial.print(lux);
        Serial.println(" lx");
    }

    return values;
}
