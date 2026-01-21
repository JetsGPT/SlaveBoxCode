# 📦 SlaveBox - ESP32 Sensor Hub (RoomSense)

<div align="center">

![ESP32](https://img.shields.io/badge/ESP32-Microcontroller-blue?style=for-the-badge&logo=espressif)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Enabled-orange?style=for-the-badge&logo=platformio)
![C++](https://img.shields.io/badge/C++-Code-00599C?style=for-the-badge&logo=cplusplus)
![I2C](https://img.shields.io/badge/I2C-Protocol-green?style=for-the-badge)
![BLE](https://img.shields.io/badge/BLE-Bluetooth_5.0-blue?style=for-the-badge&logo=bluetooth)

**A modular ESP32-based multi-sensor data acquisition system with BLE connectivity and OLED display**

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Supported Sensors](#-supported-sensors)
- [Hardware Components](#-hardware-components)
- [Project Structure](#-project-structure)
- [Getting Started](#-getting-started)
- [Usage](#-usage)
- [Button Controls & Power Management](#-button-controls--power-management)
- [BLE Communication](#-ble-communication)
- [OLED Display](#-oled-display)
- [Python BLE Bridge](#-python-ble-bridge)
- [Adding New Sensors](#-adding-new-sensors)
- [Troubleshooting](#-troubleshooting)
- [Glossary](#-glossary)
- [Contributing](#-contributing)
- [License](#-license)
- [Authors](#-authors)
- [Acknowledgments](#-acknowledgments)

---

## 🔍 Overview

SlaveBox (branded as **RoomSense**) is an ESP32-based sensor hub designed for environmental monitoring and data acquisition. It provides a clean, modular architecture for managing multiple I2C sensors simultaneously with automatic scanning, reading, and data aggregation. Data can be transmitted wirelessly via BLE and displayed on an integrated OLED screen.

### Why SlaveBox?

- ✅ **Modular Design** - Easy to add/remove sensors
- ✅ **Clean Code** - Helper functions keep main code simple
- ✅ **Auto-Discovery** - Automatically scans and identifies I2C sensors
- ✅ **Structured Data** - Returns organized sensor data in nested maps
- ✅ **BLE Connectivity** - Wireless data transmission with secure pairing
- ✅ **OLED Display** - Real-time status and pairing PIN display
- ✅ **Power Management** - Auto-sleep display with button wake and always-on mode
- ✅ **Debug Support** - Built-in debug output for troubleshooting

---

## ✨ Features

- **Multi-Sensor Support**: Simultaneously read from multiple I2C sensors
- **Automatic I2C Scanning**: Detects connected sensors on startup
- **Modular Helper System**: Each sensor has dedicated helper files
- **Centralized Management**: SensorManager handles all sensor operations
- **Structured Data Output**: Returns data as `std::map<String, std::map<String, float>>`
- **BLE Data Transmission**: Stream sensor data wirelessly to connected devices
- **Secure BLE Pairing**: PIN-based pairing with on-screen display
- **OLED Display**: 128x32 SSD1306 screen for status and pairing info
- **Visual Progress Indicator**: Circular countdown timer during pairing
- **Button Controls**: Short press to wake display, long press for always-on mode
- **Screen Power Management**: Auto-sleep after 10 seconds of inactivity
- **Debug Mode**: Toggle verbose output for development

---

## 🌡️ Supported Sensors

| Sensor | Type | I2C Address | Measurements |
|--------|------|-------------|--------------|
| **BME280** | Environmental | `0x77` | Temperature, Humidity, Pressure |
| **SGP30** | Air Quality | `0x58` | eCO2, TVOC |
| **BH1750** | Light Sensor | `0x23` | Ambient Light (lux) |

---

## 🔩 Hardware Components

| Component | Description | I2C Address |
|-----------|-------------|-------------|
| **ESP32 WROOM** | Main microcontroller | - |
| **SSD1306 OLED** | 128x32 pixel display | `0x3C` |
| **BME280** | Environmental sensor | `0x77` |
| **SGP30** | Air quality sensor | `0x58` |
| **BH1750** | Light sensor | `0x23` |
| **Push Button** | User input (GPIO 4) | - |

---

## 📁 Project Structure

```
SlaveBox/
├── include/
│   ├── Bme280Helper.h         # BME280 sensor interface
│   ├── Sgp30Helper.h          # SGP30 sensor interface
│   ├── Bh1750Helper.h         # BH1750 sensor interface
│   ├── SensorManager.h        # Central sensor management
│   ├── BLEHelper.h            # Bluetooth Low Energy communication
│   ├── ScreenHelper.h         # OLED display control
│   ├── DisplayHelper.h        # Sensor data display formatting
│   ├── ButtonHandler.h        # Debounced button input handling
│   ├── ScreenPowerManager.h   # Display auto-sleep/wake management
│   └── runSetup.h             # Initialization routines
├── src/
│   ├── main.cpp               # Main application entry
│   ├── Bme280Helper.cpp       # BME280 implementation
│   ├── Sgp30Helper.cpp        # SGP30 implementation
│   ├── Bh1750Helper.cpp       # BH1750 implementation
│   ├── SensorManager.cpp      # Sensor manager implementation
│   ├── BLEHelper.cpp          # BLE server and data transmission
│   ├── ScreenHelper.cpp       # OLED display functions
│   ├── DisplayHelper.cpp      # Sensor data display logic
│   ├── ButtonHandler.cpp      # Button input processing
│   ├── ScreenPowerManager.cpp # Screen power state management
│   └── runSetup.cpp           # Hardware initialization
├── platformio.ini             # PlatformIO configuration
├── requirements.txt           # Python dependencies for BLE bridge
└── README.md                  # This file
```

---

## 🚀 Getting Started

### Prerequisites

- **Hardware**:
  - ESP32 Development Board (WROOM recommended)
  - SSD1306 OLED Display (128x32, I2C)
  - Supported I2C sensors (BME280, SGP30, BH1750)
  - Push button (momentary, normally open)
  - Jumper wires for connections

- **Software**:
  - [PlatformIO IDE](https://platformio.org/) or [PlatformIO CLI](https://docs.platformio.org/en/latest/core/installation.html)
  - USB cable for programming

### Dependencies (Auto-installed via PlatformIO)

| Library | Version | Purpose |
|---------|---------|---------|
| Adafruit BME280 | ^2.3.0 | Temperature/Humidity/Pressure sensor |
| Adafruit SGP30 | ^2.0.3 | Air quality sensor |
| BH1750 (claws) | ^1.3.0 | Light sensor |
| ArduinoJson | ^7.2.1 | JSON serialization for BLE data |
| Adafruit GFX | ^1.12.4 | Graphics library for OLED |
| Adafruit SSD1306 | ^2.5.16 | OLED display driver |

### Wiring

All sensors use I2C protocol. Connect as follows:

| ESP32 Pin | Connection |
|-----------|------------|
| GPIO 21 (SDA) | SDA (all I2C devices) |
| GPIO 22 (SCL) | SCL (all I2C devices) |
| GPIO 4 | Button (other side to GND) |
| 3.3V | VCC (all sensors & display) |
| GND | GND (all devices) |

> **Note**: Multiple I2C devices can share the same SDA/SCL bus. The OLED display (0x3C) and all sensors connect to the same I2C bus. The button uses INPUT_PULLUP mode, so connect between GPIO 4 and GND.

### Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/JetsGPT/SlaveBoxCode.git
   cd SlaveBoxCode
   ```

2. **Open in PlatformIO**:
   ```bash
   cd SlaveBox
   pio run
   ```

3. **Upload to ESP32**:
   ```bash
   pio run --target upload
   ```

4. **Monitor Serial Output**:
   ```bash
   pio device monitor
   ```

---

## 💻 Usage

### Basic Example

The main loop automatically scans and reads all sensors, handles button input, manages display power, and transmits data over BLE:

```cpp
#include <Arduino.h>
#include "runSetup.h"
#include "SensorManager.h"
#include "BLEHelper.h"
#include "DisplayHelper.h"
#include "ButtonHandler.h"
#include "ScreenPowerManager.h"

// Button on GPIO 4 (connect button between GPIO4 and GND)
ButtonHandler button(4);

void setup() {
  runSetup();
  initializeSensors();
  button.begin();
  screenPowerManager.begin();  // Starts with screen OFF
  delay(1000);
}

void loop() {
  // Update button state (must be called every loop)
  button.update();
  
  // Update screen power manager (handles auto-timeout)
  screenPowerManager.update();
  
  // Handle button presses
  if (button.wasLongPressed()) {
    screenPowerManager.toggleAlwaysOn();  // Long press: toggle always-on
  } else if (button.wasPressed()) {
    screenPowerManager.wake();  // Short press: wake screen
  }
  
  // Get all sensor data
  std::map<String, std::map<String, float>> sensorData = scanAndReadAllSensors(true);

  // Display when screen is on and not showing pairing PIN
  if (screenPowerManager.isScreenOn() && !bleHelper.isPairing()) {
    if (sensorData.empty()) {
      displayNoSensors();
    } else {
      displaySensorData(sensorData);
    }
  }

  // Send data over BLE if connected
  if (bleHelper.isConnected()) {
    bleHelper.sendMap(sensorData);
  }
}
```

### Output Format

The `scanAndReadAllSensors()` function returns data in this structure:

```cpp
{
  "BME280": {
    "temperature": 25.5,    // °C
    "humidity": 60.0,       // %
    "pressure": 1013.25     // hPa
  },
  "SGP30": {
    "eCO2": 400,           // ppm
    "TVOC": 0              // ppb
  },
  "BH1750": {
    "light": 250.5         // lux
  }
}
```

### Serial Monitor Output

```
=== Scanning I2C bus ===

✓ Found BME280 at 0x77
  Readings:
    temperature: 25.50
    humidity: 60.00
    pressure: 1013.25

✓ Found SGP30 at 0x58
  Readings:
    eCO2: 400
    TVOC: 0

✓ Found BH1750 at 0x23
  Readings:
    light: 250.50

=== Scan complete ===
```

---

### SensorManager Functions

#### `void initializeSensors()`
Initializes the I2C bus and prepares sensors for reading.

```cpp
void setup() {
  initializeSensors();
}
```

#### `std::map<String, std::map<String, float>> scanAndReadAllSensors(bool debug = true)`
Scans all known sensors and returns their readings.

**Parameters**:
- `debug` - Enable/disable serial output (default: `true`)

**Returns**: Nested map with structure `{SensorName: {ValueType: Value}}`

```cpp
auto data = scanAndReadAllSensors(true);  // With debug output
auto data = scanAndReadAllSensors(false); // Silent mode
```

#### `SensorInfo* getSensorByAddress(uint8_t addr)`
Retrieves sensor information by I2C address.

```cpp
SensorInfo* sensor = getSensorByAddress(0x77); // Get BME280 info
```

### Individual Sensor Helpers

#### BME280
```cpp
std::map<String, float> getValues_BME280(uint8_t addr, bool debug);
```
Returns: `{"temperature", "humidity", "pressure"}`

#### SGP30
```cpp
std::map<String, float> getValues_SGP30(bool debug);
```
Returns: `{"eCO2", "TVOC"}`

#### BH1750
```cpp
std::map<String, float> getValues_BH1750(uint8_t addr, bool debug);
```
Returns: `{"light"}`

---

## 🔘 Button Controls & Power Management

SlaveBox includes a button interface for controlling the OLED display power state.

### Button Wiring

Connect a momentary push button between **GPIO 4** and **GND**. The internal pull-up resistor is enabled automatically.

### Button Functions

| Action | Function |
|--------|----------|
| **Short Press** | Wake the display and reset the auto-sleep timer |
| **Long Press** (2+ seconds) | Toggle "Always On" mode (disables auto-sleep) |

### Screen Power Management

The display uses intelligent power management to conserve energy:

- **Default State**: Screen starts OFF
- **Auto-Sleep**: Screen turns off after 10 seconds of inactivity
- **Wake on Pairing**: Screen automatically wakes to show BLE pairing PIN
- **Always-On Mode**: Disables auto-sleep (toggled via long press)

### ButtonHandler API

```cpp
ButtonHandler button(4);  // GPIO pin 4, 50ms debounce, 2000ms long press

button.begin();           // Initialize GPIO
button.update();          // Call every loop iteration

if (button.wasPressed()) { ... }      // Short press detected
if (button.wasLongPressed()) { ... }  // Long press detected
if (button.isHeld()) { ... }          // Button currently held
```

### ScreenPowerManager API

```cpp
screenPowerManager.begin();     // Initialize (screen starts OFF)
screenPowerManager.update();    // Call every loop (checks timeout)
screenPowerManager.wake();      // Turn on and reset timer
screenPowerManager.sleep();     // Turn off immediately
screenPowerManager.toggleAlwaysOn();  // Toggle always-on mode

if (screenPowerManager.isScreenOn()) { ... }
if (screenPowerManager.isAlwaysOn()) { ... }
```

---

## 📡 BLE Communication

SlaveBox includes a BLE (Bluetooth Low Energy) server for wireless data transmission.

### BLE Service & Characteristics

| UUID | Name | Description |
|------|------|-------------|
| `cfa59c64-aeaf-42ac-bf8d-bc4a41ef5b0c` | Service | Main sensor service |
| `49c92b70-42f5-49c3-bc38-5fe05b3df8e0` | Sensor Data | JSON-formatted sensor readings |
| `3bee5811-4c6c-449a-b368-0b1391c6c1dc` | Sensor Type | Primary sensor type identifier |
| `9d62dc0c-b4ef-40c4-9383-15bdc16870de` | Box ID | Device identifier |

### BLE Helper Functions

```cpp
// Initialize BLE with device name and box ID
bleHelper.begin("RoomSense-Sensor-01", "box_room_001");

// Check if a client is connected
if (bleHelper.isConnected()) {
    // Send sensor data as JSON
    bleHelper.sendMap(sensorData);
}

// Check if pairing is in progress
if (bleHelper.isPairing()) {
    // PIN is being displayed on screen
}
```

### Secure Pairing

When a device attempts to pair:
1. A 6-digit PIN is generated and displayed on the OLED screen
2. A 30-second countdown timer shows remaining time
3. User enters the PIN on their phone/device
4. Screen shows "SUCCESS!" or "FAILED!" based on result

---

## 🖥️ OLED Display

The 128x32 SSD1306 OLED display provides visual feedback.

### Screen Functions

```cpp
// Initialize the display
bool initScreen();

// Display header and value
updateScreen("Temperature", "24.5 C", true);

// Display with circular progress indicator (for pairing countdown)
updateScreenWithProgress("ENTER PIN (25s)", "123-456", 83);

// Clear the display
clearScreen();

// Control display power (hardware level)
setDisplayPower(true);   // Turn on
setDisplayPower(false);  // Turn off
```

### DisplayHelper Functions

```cpp
// Cycle through all sensor metrics with formatted display
displaySensorData(sensorData, 2000);  // 2 second per metric

// Show "No Sensors" message
displayNoSensors();
```

### Display Modes

| Mode | Description |
|------|-------------|
| Header + Value | Shows title with divider line and large value text |
| Value Only | Large centered text (when `showHeader = false`) |
| Progress Mode | Header, value, and circular countdown indicator |

---

## 🐍 Python BLE Bridge

A Python-based BLE to MQTT bridge is available for integrating with home automation systems.

### Python Dependencies

Install the required packages:

```bash
pip install -r requirements.txt
```

**requirements.txt**:
```
bleak>=0.21.0
aiomqtt>=1.2.0
```

| Package | Purpose |
|---------|---------|
| **bleak** | BLE client library for Python |
| **aiomqtt** | Async MQTT client for publishing sensor data |

---

## 🔧 Adding New Sensors

Follow these steps to add a new I2C sensor:

### 1. Create Helper Files

**include/NewSensorHelper.h**:
```cpp
#ifndef NEWSENSORHELPER_H
#define NEWSENSORHELPER_H

#include <Arduino.h>
#include <map>

std::map<String, float> getValues_NewSensor(uint8_t addr, bool debug);

#endif
```

**src/NewSensorHelper.cpp**:
```cpp
#include "NewSensorHelper.h"
#include <Wire.h>
// Include sensor library

static bool initialized = false;

std::map<String, float> getValues_NewSensor(uint8_t addr, bool debug) {
    if (!initialized) {
        // Initialize sensor
        initialized = true;
    }

    std::map<String, float> values;
    // Read sensor values
    values["measurement"] = sensorValue;
    
    return values;
}
```

### 2. Update SensorManager.cpp

Add to includes:
```cpp
#include "NewSensorHelper.h"
```

Add wrapper function:
```cpp
static std::map<String, float> readNewSensor(uint8_t addr, bool debug) {
  return getValues_NewSensor(addr, debug);
}
```

Add to sensor array:
```cpp
static SensorInfo sensors[] = {
  // ... existing sensors ...
  {0xYY, "NewSensor", readNewSensor}
};
```

### 3. Done! 
Your sensor will now be automatically scanned and read.

---

## ❓ Troubleshooting

### Common Issues

| Problem | Solution |
|---------|----------|
| **No sensors detected** | Check I2C wiring (SDA to GPIO 21, SCL to GPIO 22). Verify 3.3V power. |
| **OLED not working** | Confirm I2C address is 0x3C. Check for loose connections. |
| **BLE not advertising** | Ensure no other device is connected. Restart the ESP32. |
| **BLE pairing fails** | Enter PIN within 30 seconds. Ensure phone's Bluetooth is on. |
| **Button not responding** | Verify button is between GPIO 4 and GND. Check for bad contacts. |
| **Display stays off** | Press button to wake. Long press to disable auto-sleep. |
| **Sensor readings are wrong** | Allow sensors to warm up (SGP30 needs 15 seconds). |

### I2C Scanner

If sensors aren't detected, run an I2C scanner to verify device addresses:

```cpp
#include <Wire.h>

void setup() {
  Wire.begin(21, 22);
  Serial.begin(115200);
  
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("Found device at 0x%02X\n", addr);
    }
  }
}

void loop() {}
```

### Debug Mode

Enable debug output to see detailed sensor readings:

```cpp
auto data = scanAndReadAllSensors(true);  // Enables verbose output
```

---

## 📖 Glossary

| Term | Definition |
|------|------------|
| **I2C** | Inter-Integrated Circuit - A two-wire serial communication protocol used to connect sensors |
| **BLE** | Bluetooth Low Energy - A wireless protocol optimized for low power consumption |
| **SDA** | Serial Data Line - The I2C data signal wire |
| **SCL** | Serial Clock Line - The I2C clock signal wire |
| **eCO2** | Equivalent Carbon Dioxide - An estimation of CO2 levels based on VOC measurements (ppm) |
| **TVOC** | Total Volatile Organic Compounds - Measure of air quality pollutants (ppb) |
| **lux** | Unit of illuminance measuring light intensity |
| **hPa** | Hectopascal - Unit of atmospheric pressure (1 hPa = 1 mbar) |
| **UUID** | Universally Unique Identifier - Used to identify BLE services and characteristics |
| **OLED** | Organic Light-Emitting Diode - Display technology used for the screen |

---

## 🤝 Contributing

Contributions are welcome! Here's how you can help:

1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/AmazingFeature`)
3. **Commit** your changes (`git commit -m 'Add some AmazingFeature'`)
4. **Push** to the branch (`git push origin feature/AmazingFeature`)
5. **Open** a Pull Request

### Guidelines

- Follow the existing code style
- Add comments for complex logic
- Update documentation for new features
- Test on actual hardware when possible
- Create sensor helpers following the existing pattern

### Ideas for Contributions

- Support for additional sensors
- Web-based configuration interface
- MQTT direct publishing from ESP32
- Data logging to SD card
- Multi-room mesh networking

---

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 Julian

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 👥 Authors

- **Julian** - *Initial work*

---

## 🙏 Acknowledgments

- Adafruit for sensor and display libraries
- bblanchon for ArduinoJson library
- claws for BH1750 library
- PlatformIO for the excellent development platform
- ESP32 community for support and documentation

---

<div align="center">

**Made with ❤️ for the IoT community**

[⬆ Back to Top](#-slavebox---esp32-sensor-hub-roomsense)

</div>
