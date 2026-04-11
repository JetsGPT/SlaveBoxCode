#include "ScreenHelper.h"

// Create display instance
static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
static bool screenInitialized = false;

bool initScreen() {
    // Don't reinitialize Wire - it's already done in runSetup
    // Wire.begin(SCREEN_I2C_SDA, SCREEN_I2C_SCL);
    
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_I2C_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        screenInitialized = false;
        return false;
    }
    
    screenInitialized = true;
    display.clearDisplay();
    display.display();
    return true;
}

// Draw a circular progress indicator (arc that shrinks as progress decreases)
// percentage: 0-100, where 100 = full circle, 0 = empty
// centerX, centerY: center of the circle
// radius: radius of the circle
void drawProgressCircle(int centerX, int centerY, int radius, int percentage) {
    if (!screenInitialized) return;
    
    // Draw arc from top (270 degrees) clockwise
    // percentage 100 = full circle, 0 = nothing
    float startAngle = -90.0;  // Start from top (12 o'clock)
    float endAngle = startAngle + (360.0 * percentage / 100.0);
    
    // Draw the arc pixel by pixel
    for (float angle = startAngle; angle < endAngle; angle += 2.0) {
        float radians = angle * PI / 180.0;
        int x = centerX + radius * cos(radians);
        int y = centerY + radius * sin(radians);
        display.drawPixel(x, y, SSD1306_WHITE);
        
        // Draw inner pixels for thicker arc
        int x2 = centerX + (radius - 1) * cos(radians);
        int y2 = centerY + (radius - 1) * sin(radians);
        display.drawPixel(x2, y2, SSD1306_WHITE);
    }
}

void updateScreenWithProgress(String headerText, String valueText, int percentage) {
    if (!screenInitialized) {
        Serial.println("Screen not initialized, skipping update");
        return;
    }
    
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Draw Header (Small)
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(headerText);

    // Draw Divider Line (shorter to make room for circle)
    display.drawLine(0, 10, 100, 10, SSD1306_WHITE);

    // Draw Value (Medium) - leave room for progress circle
    display.setTextSize(2);
    display.setCursor(0, 14);
    display.println(valueText);
    
    // Draw progress circle on the right side
    // Center at x=115, y=16 (middle right of screen), radius 12
    drawProgressCircle(115, 16, 12, percentage);

    display.display();
}

void updateScreen(String headerText, String valueText, bool showHeader) {
    // Safety check - don't use display if not initialized
    if (!screenInitialized) {
        Serial.println("Screen not initialized, skipping update");
        return;
    }
    
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    if (showHeader) {
        // MODE A: Header + Value

        // 1. Draw Header (Small)
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println(headerText);

        // 2. Draw Divider Line
        display.drawLine(0, 10, 128, 10, SSD1306_WHITE);

        // 3. Draw Value (Medium)
        display.setTextSize(2);
        display.setCursor(0, 14);
        display.println(valueText);

    } else {
        // MODE B: Value Only (Huge)

        // Draw Value (Large Size 3)
        display.setTextSize(3);
        display.setCursor(0, 5);
        display.println(valueText);
    }

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
