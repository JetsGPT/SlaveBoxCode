#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <Arduino.h>

/**
 * @brief Handles debounced button input with short and long press detection
 * 
 * Usage:
 *   ButtonHandler button(GPIO_PIN);
 *   button.begin();
 *   
 *   // In loop:
 *   button.update();
 *   if (button.wasPressed()) { ... }
 *   if (button.wasLongPressed()) { ... }
 */
class ButtonHandler {
public:
    /**
     * @param pin GPIO pin number (will use INPUT_PULLUP, button should connect to GND)
     * @param debounceMs Debounce time in milliseconds (default 50ms)
     * @param longPressMs Time to register as long press (default 2000ms)
     */
    ButtonHandler(uint8_t pin, unsigned long debounceMs = 50, unsigned long longPressMs = 2000);
    
    /** Initialize the GPIO pin */
    void begin();
    
    /** Call every loop iteration to update button state */
    void update();
    
    /** Returns true once after a short press is detected (resets after read) */
    bool wasPressed();
    
    /** Returns true once after a long press is detected (resets after read) */
    bool wasLongPressed();
    
    /** Returns true if button is currently held down */
    bool isHeld();

private:
    uint8_t _pin;
    unsigned long _debounceMs;
    unsigned long _longPressMs;
    
    bool _lastRawState;
    bool _stableState;
    bool _wasPressed;
    bool _wasLongPressed;
    bool _longPressTriggered;
    
    unsigned long _lastDebounceTime;
    unsigned long _pressStartTime;
};

#endif // BUTTONHANDLER_H
