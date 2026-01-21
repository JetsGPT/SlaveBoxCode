#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(uint8_t pin, unsigned long debounceMs, unsigned long longPressMs)
    : _pin(pin)
    , _debounceMs(debounceMs)
    , _longPressMs(longPressMs)
    , _lastRawState(HIGH)
    , _stableState(HIGH)
    , _wasPressed(false)
    , _wasLongPressed(false)
    , _longPressTriggered(false)
    , _lastDebounceTime(0)
    , _pressStartTime(0)
{}

void ButtonHandler::begin() {
    pinMode(_pin, INPUT_PULLUP);
    _lastRawState = digitalRead(_pin);
    _stableState = _lastRawState;
}

void ButtonHandler::update() {
    bool currentRaw = digitalRead(_pin);
    unsigned long now = millis();
    
    // Debounce: if reading changed, reset timer
    if (currentRaw != _lastRawState) {
        _lastDebounceTime = now;
        _lastRawState = currentRaw;
    }
    
    // Only accept state change after debounce period
    if ((now - _lastDebounceTime) >= _debounceMs) {
        // State has been stable long enough
        if (currentRaw != _stableState) {
            bool wasHigh = _stableState;
            _stableState = currentRaw;
            
            // Button pressed (HIGH -> LOW with INPUT_PULLUP)
            if (wasHigh && !_stableState) {
                _pressStartTime = now;
                _longPressTriggered = false;
            }
            
            // Button released (LOW -> HIGH)
            if (!wasHigh && _stableState) {
                // Only register short press if long press wasn't already triggered
                if (!_longPressTriggered) {
                    _wasPressed = true;
                }
            }
        }
    }
    
    // Check for long press while button is held
    if (_stableState == LOW && !_longPressTriggered) {
        if ((now - _pressStartTime) >= _longPressMs) {
            _wasLongPressed = true;
            _longPressTriggered = true;  // Prevent retriggering
        }
    }
}

bool ButtonHandler::wasPressed() {
    bool result = _wasPressed;
    _wasPressed = false;
    return result;
}

bool ButtonHandler::wasLongPressed() {
    bool result = _wasLongPressed;
    _wasLongPressed = false;
    return result;
}

bool ButtonHandler::isHeld() {
    return (_stableState == LOW);
}
