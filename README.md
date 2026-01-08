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
- [BLE Communication](#-ble-communication)
- [OLED Display](#-oled-display)
- [Adding New Sensors](#-adding-new-sensors)

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

---

## 📁 Project Structure

```
SlaveBox/
├── include/
│   ├── Bme280Helper.h      # BME280 sensor interface
│   ├── Sgp30Helper.h       # SGP30 sensor interface
│   ├── Bh1750Helper.h      # BH1750 sensor interface
│   ├── SensorManager.h     # Central sensor management
│   ├── BLEHelper.h         # Bluetooth Low Energy communication
│   ├── ScreenHelper.h      # OLED display control
│   └── runSetup.h          # Initialization routines
├── src/
│   ├── main.cpp            # Main application entry
│   ├── Bme280Helper.cpp    # BME280 implementation
│   ├── Sgp30Helper.cpp     # SGP30 implementation
│   ├── Bh1750Helper.cpp    # BH1750 implementation
│   ├── SensorManager.cpp   # Sensor manager implementation
│   ├── BLEHelper.cpp       # BLE server and data transmission
│   ├── ScreenHelper.cpp    # OLED display functions
│   └── runSetup.cpp        # Hardware initialization
├── platformio.ini          # PlatformIO configuration
└── README.md               # This file
```

---

## 🚀 Getting Started

### Prerequisites

- **Hardware**:
  - ESP32 Development Board (WROOM recommended)
  - SSD1306 OLED Display (128x32, I2C)
  - Supported I2C sensors (BME280, SGP30, BH1750)
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

| ESP32 Pin | Sensor Pin |
|-----------|------------|
| GPIO 21 (SDA) | SDA |
| GPIO 22 (SCL) | SCL |
| 3.3V | VCC |
| GND | GND |

> **Note**: Multiple I2C devices can share the same SDA/SCL bus. The OLED display (0x3C) and all sensors connect to the same I2C bus.

### Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/yourusername/SlaveBoxCode.git
   cd SlaveBoxCode
   ```

2. **Open in PlatformIO**:
   ```bash
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

The main loop automatically scans and reads all sensors, then transmits data over BLE:

```cpp
#include <Arduino.h>
#include "runSetup.h"
#include "SensorManager.h"
#include "BLEHelper.h"

void setup() {
  runSetup();
  initializeSensors();
  delay(1000);
}

void loop() {
  // Get all sensor data as a structured dictionary
  std::map<String, std::map<String, float>> sensorData = scanAndReadAllSensors(true);
  
  // Send sensor data over BLE (if connected)
  if (bleHelper.isConnected()) {
    bleHelper.sendMap(sensorData);
  }
  
  delay(5000);
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
```

### Display Modes

| Mode | Description |
|------|-------------|
| Header + Value | Shows title with divider line and large value text |
| Value Only | Large centered text (when `showHeader = false`) |
| Progress Mode | Header, value, and circular countdown indicator |

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

[⬆ Back to Top](#-slavebox---esp32-sensor-hub)

</div>
