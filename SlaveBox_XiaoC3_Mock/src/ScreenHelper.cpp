// ============================================================
// ScreenHelper.cpp  (mock version)
// Identical implementation to the real ScreenHelper — the physical
// SSD1306 OLED is actually supported on the XIAO ESP32-C3, so this
// file is a direct port rather than a stub.
// ============================================================

#include "ScreenHelper.h"
#include <Wire.h>
#include <math.h>

// Create display instance (no RESET pin, OLED RST = -1)
static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
static bool screenInitialized = false;

bool initScreen() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDRESS)) {
        Serial.println(F("[MOCK] SSD1306 allocation failed (OLED not found at 0x3C)"));
        screenInitialized = false;
        return false;
    }
    screenInitialized = true;
    display.clearDisplay();
    display.display();
    Serial.println("[MOCK] SSD1306 OLED initialized ✅");
    return true;
}

// Draw a circular countdown arc (same algorithm as real code)
static void drawProgressCircle(int cx, int cy, int radius, int percentage) {
    if (!screenInitialized) return;
    float startAngle = -90.0f;
    float endAngle   = startAngle + (360.0f * percentage / 100.0f);
    for (float angle = startAngle; angle < endAngle; angle += 2.0f) {
        float rad = angle * (float)PI / 180.0f;
        int x  = cx + (int)(radius * cos(rad));
        int y  = cy + (int)(radius * sin(rad));
        display.drawPixel(x, y, SSD1306_WHITE);
        int x2 = cx + (int)((radius - 1) * cos(rad));
        int y2 = cy + (int)((radius - 1) * sin(rad));
        display.drawPixel(x2, y2, SSD1306_WHITE);
    }
}

void updateScreen(String headerText, String valueText, bool showHeader) {
    if (!screenInitialized) {
        Serial.println("[MOCK OLED] Screen not initialized, skipping update");
        return;
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    if (showHeader) {
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println(headerText);
        display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
        display.setTextSize(2);
        display.setCursor(0, 14);
        display.println(valueText);
    } else {
        display.setTextSize(3);
        display.setCursor(0, 5);
        display.println(valueText);
    }
    display.display();
}

void updateScreenWithProgress(String headerText, String valueText, int percentage) {
    if (!screenInitialized) {
        Serial.println("[MOCK OLED] Screen not initialized, skipping");
        return;
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(headerText);
    display.drawLine(0, 10, 100, 10, SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(0, 14);
    display.println(valueText);
    drawProgressCircle(115, 16, 12, percentage);
    display.display();
}

void clearScreen() {
    if (!screenInitialized) return;
    display.clearDisplay();
    display.display();
}

void setDisplayPower(bool on) {
    if (!screenInitialized) return;
    if (on) {
        display.ssd1306_command(SSD1306_DISPLAYON);
    } else {
        display.ssd1306_command(SSD1306_DISPLAYOFF);
    }
}
