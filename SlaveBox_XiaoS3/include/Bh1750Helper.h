#ifndef BH1750HELPER_H
#define BH1750HELPER_H

#include <Arduino.h>
#include <map>

std::map<String, float> getValues_BH1750(uint8_t addr, bool debug);

#endif
