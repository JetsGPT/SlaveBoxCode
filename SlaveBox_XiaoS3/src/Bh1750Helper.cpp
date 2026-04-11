#include "Bh1750Helper.h"
#include <Wire.h>
#include <BH1750.h>
#include <cmath>

static BH1750 lightMeter;
static bool initialized = false;
static bool initFailed = false;

#define SENSOR_ERROR_VALUE -999.0f
#define BH1750_LUX_MAX 65535.0f  // Maximum sensor reading

// Return Name - Value in a Map
// addr - Sensor's address (default 0x23)
std::map<String, float> getValues_BH1750(uint8_t addr, bool debug) {
    std::map<String, float> values;

    // Don't retry if init already failed
    if (initFailed) {
        values["light"] = SENSOR_ERROR_VALUE;
        return values;
    }

    if (!initialized) {
        if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, addr, &Wire)) {
            if (debug) {
                Serial.println("Could not find a valid BH1750 sensor, check wiring!");
            }
            // Return error value instead of hanging - allows device to continue
            initFailed = true;
            values["light"] = SENSOR_ERROR_VALUE;
            return values;
        }
        if (debug) Serial.println("BH1750 sensor initialized ✅");
        initialized = true;
    }

    float lux = lightMeter.readLightLevel();
    
    // Validate reading
    if (isnan(lux) || lux < 0.0f || lux > BH1750_LUX_MAX) {
        values["light"] = SENSOR_ERROR_VALUE;
        if (debug) Serial.println("BH1750: Invalid light reading");
    } else {
        values["light"] = lux;
        if (debug) {
            Serial.print("Light: ");
            Serial.print(lux);
            Serial.println(" lx");
        }
    }

    return values;
}
