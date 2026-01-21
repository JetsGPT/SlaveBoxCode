#ifndef SCREENPOWERMANAGER_H
#define SCREENPOWERMANAGER_H

#include <Arduino.h>

/**
 * @brief Manages OLED screen power state with auto-timeout and always-on mode
 * 
 * Usage:
 *   ScreenPowerManager screenPower(10000);  // 10 second timeout
 *   
 *   // In loop:
 *   screenPower.update();
 *   if (buttonPressed) screenPower.wake();
 *   if (longPress) screenPower.toggleAlwaysOn();
 *   if (screenPower.isScreenOn()) { displayMetrics(); }
 */
class ScreenPowerManager {
public:
    /**
     * @param timeoutMs Auto-sleep timeout in milliseconds (default 10000 = 10 seconds)
     */
    ScreenPowerManager(unsigned long timeoutMs = 10000);
    
    /** Initialize - starts with screen OFF */
    void begin();
    
    /** Call every loop iteration to check timeout */
    void update();
    
    /** Turn screen on and reset timeout timer */
    void wake();
    
    /** Turn screen off immediately */
    void sleep();
    
    /** Check if screen is currently on */
    bool isScreenOn();
    
    /** Toggle always-on mode (disables auto-sleep) */
    void toggleAlwaysOn();
    
    /** Check if always-on mode is enabled */
    bool isAlwaysOn();

private:
    unsigned long _timeoutMs;
    unsigned long _lastWakeTime;
    bool _screenOn;
    bool _alwaysOn;
    
    void setScreenPower(bool on);
};

// Global instance for easy access from other modules
extern ScreenPowerManager screenPowerManager;

#endif // SCREENPOWERMANAGER_H
