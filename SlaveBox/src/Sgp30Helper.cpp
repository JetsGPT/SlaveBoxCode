#include "SGP30Helper.h"
#include <Wire.h>
#include "Adafruit_SGP30.h"

static Adafruit_SGP30 sgp;
static bool initialized = false;

// address is set automatically for SGP30
std::map<String, float> getValues_SGP30(bool debug) {
    if (!initialized) {
        if (!sgp.begin(&Wire)) {
            if (debug) Serial.println("Could not find a valid SGP30 sensor, check wiring!");
            while (1);
        }
        if (debug) Serial.println("SGP30 sensor initialized ✅");
        initialized = true;
    }

    std::map<String, float> values;

    if (!sgp.IAQmeasure()) {
        if (debug) Serial.println("Measurement failed");
        values["eCO2"] = -1;
        values["TVOC"] = -1;
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
