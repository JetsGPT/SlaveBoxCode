#ifndef SGP30HELPER_H
#define SGP30HELPER_H

#include <Arduino.h>
#include <map>

std::map<String, float> getValues_SGP30(bool debug);

#endif
