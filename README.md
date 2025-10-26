# 📦 SlaveBox - ESP32 Sensor Hub

<div align="center">

![ESP32](https://img.shields.io/badge/ESP32-Microcontroller-blue?style=for-the-badge&logo=espressif)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Enabled-orange?style=for-the-badge&logo=platformio)
![C++](https://img.shields.io/badge/C++-Code-00599C?style=for-the-badge&logo=cplusplus)
![I2C](https://img.shields.io/badge/I2C-Protocol-green?style=for-the-badge)

**A modular ESP32-based multi-sensor data acquisition system**

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Supported Sensors](#-supported-sensors)
- [Project Structure](#-project-structure)
- [Getting Started](#-getting-started)
- [Usage](#-usage)
- [API Reference](#-api-reference)
- [Adding New Sensors](#-adding-new-sensors)
- [Contributing](#-contributing)

---

## 🔍 Overview

SlaveBox is an ESP32-based sensor hub designed for environmental monitoring and data acquisition. It provides a clean, modular architecture for managing multiple I2C sensors simultaneously with automatic scanning, reading, and data aggregation.

### Why SlaveBox?

- ✅ **Modular Design** - Easy to add/remove sensors
- ✅ **Clean Code** - Helper functions keep main code simple
- ✅ **Auto-Discovery** - Automatically scans and identifies I2C sensors
- ✅ **Structured Data** - Returns organized sensor data in nested maps
- ✅ **Debug Support** - Built-in debug output for troubleshooting

---

## ✨ Features

- **Multi-Sensor Support**: Simultaneously read from multiple I2C sensors
- **Automatic I2C Scanning**: Detects connected sensors on startup
- **Modular Helper System**: Each sensor has dedicated helper files
- **Centralized Management**: SensorManager handles all sensor operations
- **Structured Data Output**: Returns data as `std::map<String, std::map<String, float>>`
- **Debug Mode**: Toggle verbose output for development

---

## 🌡️ Supported Sensors

| Sensor | Type | I2C Address | Measurements |
|--------|------|-------------|--------------|
| **BME280** | Environmental | `0x77` | Temperature, Humidity, Pressure |
| **SGP30** | Air Quality | `0x58` | eCO2, TVOC |
| **BH1750** | Light Sensor | `0x23` | Ambient Light (lux) |

---

## 📁 Project Structure

```
SlaveBox/
├── include/
│   ├── Bme280Helper.h      # BME280 sensor interface
│   ├── Sgp30Helper.h       # SGP30 sensor interface
│   ├── Bh1750Helper.h      # BH1750 sensor interface
│   └── SensorManager.h     # Central sensor management
├── src/
│   ├── main.cpp            # Main application entry
│   ├── Bme280Helper.cpp    # BME280 implementation
│   ├── Sgp30Helper.cpp     # SGP30 implementation
│   ├── Bh1750Helper.cpp    # BH1750 implementation
│   ├── SensorManager.cpp   # Sensor manager implementation
│   └── runSetup.cpp        # Initialization routines
├── platformio.ini          # PlatformIO configuration
└── README.md               # This file
```

---

## 🚀 Getting Started

### Prerequisites

- **Hardware**:
  - ESP32 Development Board
  - Supported I2C sensors (BME280, SGP30, BH1750)
  - Jumper wires for connections

- **Software**:
  - [PlatformIO IDE](https://platformio.org/) or [PlatformIO CLI](https://docs.platformio.org/en/latest/core/installation.html)
  - USB cable for programming

### Wiring

All sensors use I2C protocol. Connect as follows:

| ESP32 Pin | Sensor Pin |
|-----------|------------|
| GPIO 21 (SDA) | SDA |
| GPIO 22 (SCL) | SCL |
| 3.3V | VCC |
| GND | GND |

> **Note**: Multiple I2C devices can share the same SDA/SCL bus.

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

The main loop automatically scans and reads all sensors:

```cpp
#include <Arduino.h>
#include "runSetup.h"
#include "SensorManager.h"

void setup() {
  runSetup();
  initializeSensors();
  delay(1000);
}

void loop() {
  // Get all sensor data as a structured dictionary
  std::map<String, std::map<String, float>> sensorData = scanAndReadAllSensors(true);
  
  // Access specific sensor values
  if (sensorData.count("BME280") > 0) {
    float temp = sensorData["BME280"]["temperature"];
    float humidity = sensorData["BME280"]["humidity"];
    Serial.print("Temperature: "); Serial.println(temp);
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

## 📚 API Reference

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

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

---

## 👥 Authors

- **Julian** - *Initial work*

---

## 🙏 Acknowledgments

- Adafruit for sensor libraries
- PlatformIO for the excellent development platform
- ESP32 community for support and documentation

---

<div align="center">

**Made with ❤️ for the IoT community**

[⬆ Back to Top](#-slavebox---esp32-sensor-hub)

</div>
