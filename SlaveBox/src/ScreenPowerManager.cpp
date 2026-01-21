#include "ScreenPowerManager.h"
#include "ScreenHelper.h"

// Global instance
ScreenPowerManager screenPowerManager(10000);  // 10 second default timeout

ScreenPowerManager::ScreenPowerManager(unsigned long timeoutMs)
    : _timeoutMs(timeoutMs)
    , _lastWakeTime(0)
    , _screenOn(false)
    , _alwaysOn(false)
{}

void ScreenPowerManager::begin() {
    // Start with screen OFF to save power
    _screenOn = false;
    _alwaysOn = false;
    setScreenPower(false);
}

void ScreenPowerManager::update() {
    // Skip timeout check if always-on mode or screen already off
    if (_alwaysOn || !_screenOn) {
        return;
    }
    
    // Check if timeout has elapsed
    unsigned long now = millis();
    if ((now - _lastWakeTime) >= _timeoutMs) {
        sleep();
    }
}

void ScreenPowerManager::wake() {
    _lastWakeTime = millis();
    
    if (!_screenOn) {
        _screenOn = true;
        setScreenPower(true);
    }
}

void ScreenPowerManager::sleep() {
    if (_screenOn) {
        _screenOn = false;
        setScreenPower(false);
    }
}

bool ScreenPowerManager::isScreenOn() {
    return _screenOn;
}

void ScreenPowerManager::toggleAlwaysOn() {
    _alwaysOn = !_alwaysOn;
    
    // If enabling always-on, make sure screen is on
    if (_alwaysOn && !_screenOn) {
        wake();
    }
    
    // Show visual feedback
    if (_alwaysOn) {
        updateScreen("Mode", "Always ON", true);
    } else {
        updateScreen("Mode", "Auto Sleep", true);
        _lastWakeTime = millis();  // Reset timer
    }
}

bool ScreenPowerManager::isAlwaysOn() {
    return _alwaysOn;
}

void ScreenPowerManager::setScreenPower(bool on) {
    setDisplayPower(on);
}
