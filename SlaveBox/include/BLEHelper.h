#ifndef BLEHELPER_H
#define BLEHELPER_H

#include <Arduino.h>
#include <map>

class BLEHelper {
public:
    BLEHelper();
    // Initialize BLE (call from runSetup.cpp)
    void begin(const String &deviceName, const String &boxId);

    // Send the nested sensor map over BLE. The map shape matches main.cpp:
    // std::map<String, std::map<String, float>>
    void sendMap(const std::map<String, std::map<String, float>> &data);

    // Check whether a BLE client is connected
    bool isConnected();

    // Check whether BLE pairing is currently in progress (PIN displayed on screen)
    bool isPairing();

private:
    // hide implementation details in the .cpp
    class Impl;
    Impl *pimpl;
};

// Provide a single global instance the rest of the code can call
extern BLEHelper bleHelper;

#endif // BLEHELPER_H
