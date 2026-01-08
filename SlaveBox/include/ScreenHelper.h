#ifndef SCREENHELPER_H
#define SCREENHELPER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Screen Dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// I2C Pins for ESP32
#define SCREEN_I2C_SDA 21
#define SCREEN_I2C_SCL 22

// I2C Address
#define SCREEN_I2C_ADDRESS 0x3C

/**
 * @brief Initialize the SSD1306 OLED display
 * @return true if initialization was successful, false otherwise
 */
bool initScreen();

/**
 * @brief Update the screen with header and value text
 * 
 * @param headerText What to write at the top (e.g., "Temperature")
 * @param valueText The main number/text to show (e.g., "24.5 C")
 * @param showHeader true = show header with divider, false = hide header & show value in large text
 */
void updateScreen(String headerText, String valueText, bool showHeader);

/**
 * @brief Update the screen with a circular progress indicator
 * 
 * @param headerText What to write at the top
 * @param valueText The main text to show
 * @param percentage Progress 0-100 (100 = full circle, 0 = empty)
 */
void updateScreenWithProgress(String headerText, String valueText, int percentage);

/**
 * @brief Clear the display
 */
void clearScreen();

#endif // SCREENHELPER_H
