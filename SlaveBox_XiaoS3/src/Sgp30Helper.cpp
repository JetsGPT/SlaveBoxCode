#include "SGP30Helper.h"
#include <Wire.h>
#include "Adafruit_SGP30.h"

static Adafruit_SGP30 sgp;
static bool initialized = false;
static bool initFailed = false;

#define SENSOR_ERROR_VALUE -999.0f

// address is set automatically for SGP30
std::map<String, float> getValues_SGP30(bool debug) {
    std::map<String, float> values;

    // Don't retry if init already failed - prevents repeated I2C errors
    if (initFailed) {
        values["eCO2"] = SENSOR_ERROR_VALUE;
        values["TVOC"] = SENSOR_ERROR_VALUE;
        return values;
    }

    if (!initialized) {
        if (!sgp.begin(&Wire)) {
            if (debug) Serial.println("Could not find a valid SGP30 sensor, check wiring!");
            // Return error values instead of hanging - allows device to continue
            initFailed = true;
            values["eCO2"] = SENSOR_ERROR_VALUE;
            values["TVOC"] = SENSOR_ERROR_VALUE;
            return values;
        }
        if (debug) Serial.println("SGP30 sensor initialized ✅");
        initialized = true;
    }

    if (!sgp.IAQmeasure()) {
        if (debug) Serial.println("SGP30: Measurement failed");
        values["eCO2"] = SENSOR_ERROR_VALUE;
        values["TVOC"] = SENSOR_ERROR_VALUE;
        return values;
    }

    values["eCO2"] = sgp.eCO2; // eCo2 - equivalent CO2 in ppm
    values["TVOC"] = sgp.TVOC; // TVOC - Total Volatile Organic Compounds in ppb

    if (debug) {
        Serial.print("eCO2 = "); Serial.print(values["eCO2"]);
        Serial.print(" ppm\t");
        Serial.print("TVOC = "); Serial.println(values["TVOC"]);
    }

    return values;
}
