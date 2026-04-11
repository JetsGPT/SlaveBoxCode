#ifndef BME280HELPER_H
#define BME280HELPER_H

#include <Arduino.h>
#include <map>

std::map<String, float> getValues_BME280(uint8_t addr, boolean debug);

#endif
