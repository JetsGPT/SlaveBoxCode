#ifndef SCREENHELPER_H
#define SCREENHELPER_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// XIAO ESP32-C3 default I2C pins (D4=SDA=GPIO6, D5=SCL=GPIO7)
#define SCREEN_WIDTH       128
#define SCREEN_HEIGHT       32
#define SCREEN_I2C_ADDRESS 0x3C

// Initialise the SSD1306 OLED; returns true on success
bool initScreen();

// Display a header + value pair, with an optional divider line
void updateScreen(String headerText, String valueText, bool showHeader);

// Display header + value + a circular countdown indicator
// percentage: 100 = full circle, 0 = empty
void updateScreenWithProgress(String headerText, String valueText, int percentage);

// Clear the display
void clearScreen();

// Hardware power on/off for the display
void setDisplayPower(bool on);

#endif // SCREENHELPER_H
