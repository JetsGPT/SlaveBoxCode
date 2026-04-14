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

- [Overview](#overview)
- [Features](#features)
- [Supported Sensors](#supported-sensors)
- [Hardware Components](#hardware-components)
- [Projects](#projects)
  - [SlaveBox\_XiaoS3 — Primary (XIAO ESP32S3)](#slavebox_xiaos3--primary-xiao-esp32s3)
  - [SlaveBox — WROOM Alternative](#slavebox--wroom-alternative)
  - [SlaveBox\_XiaoC3\_Mock — ESP32-C3 Mock Firmware](#slavebox_xiaoc3_mock--esp32-c3-mock-firmware)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Debug Mode vs Production Mode](#debug-mode-vs-production-mode)
- [Usage](#usage)
- [Button Controls & Power Management](#button-controls--power-management)
- [BLE Communication](#ble-communication)
- [OLED Display](#oled-display)
- [Python BLE Bridge](#python-ble-bridge)
- [Adding New Sensors](#adding-new-sensors)
- [Troubleshooting](#troubleshooting)
- [Glossary](#glossary)
- [Contributing](#contributing)
- [License](#license)
- [Authors](#authors)
- [Acknowledgments](#acknowledgments)

---

## 🔍 Overview

SlaveBox (branded as **RoomSense**) is an ESP32-based sensor hub designed for environmental monitoring and data acquisition. It provides a clean, modular architecture for managing multiple I2C sensors simultaneously with automatic scanning, reading, and data aggregation. Data can be transmitted wirelessly via BLE and displayed on an integrated OLED screen.

The repository contains **three projects**:

| Project | Target | Purpose |
|---------|--------|---------|
| `SlaveBox_XiaoS3` | Seeed XIAO ESP32S3 | Primary production firmware |
| `SlaveBox` | ESP32 WROOM | Alternative with button & screen-sleep support |
| `SlaveBox_XiaoC3_Mock` | Seeed XIAO ESP32-C3 | Mock firmware — no physical sensors needed |

### Why SlaveBox?

- ✅ **Modular Design** - Easy to add/remove sensors
- ✅ **Clean Code** - Helper functions keep main code simple
- ✅ **Auto-Discovery** - Automatically scans and identifies I2C sensors
- ✅ **Structured Data** - Returns organized sensor data in nested maps
- ✅ **BLE Connectivity** - Wireless data transmission with secure pairing
- ✅ **OLED Display** - Real-time status and pairing PIN display
- ✅ **Debug / Production Modes** - Switch between fast-loop dev mode and power-saving production builds
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
- **Button Controls**: Short press to wake display, long press for always-on mode *(WROOM only)*
- **Screen Power Management**: Auto-sleep after 10 seconds of inactivity *(WROOM only)*
- **Debug / Production Build Modes**: Compile-time `DEBUG_MODE` flag controls verbosity and power usage
- **Mock Firmware**: Full ESP32-C3 mock for development without hardware sensors

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
| **Seeed XIAO ESP32S3** | Primary small-form-factor controller | - |
| **Seeed XIAO ESP32-C3** | Mock firmware target | - |
| **ESP32 WROOM** | Alternative microcontroller (button + sleep support) | - |
| **SSD1306 OLED** | 128x32 pixel display | `0x3C` |
| **BME280** | Environmental sensor | `0x77` |
| **SGP30** | Air quality sensor | `0x58` |
| **BH1750** | Light sensor | `0x23` |
| **Push Button** | User input (Optional, GPIO 4 on WROOM) | - |

---

## 📁 Projects

### SlaveBox\_XiaoS3 — Primary (XIAO ESP32S3)

The main production firmware for the **Seeed Studio XIAO ESP32S3**. Identical feature set to the WROOM version minus the physical button and screen power manager (not yet wired on S3).

- I2C pins: `D4 (GPIO5)` = SDA, `D5 (GPIO6)` = SCL
- Supports: `debug` and `production` PlatformIO environments

---

### SlaveBox — WROOM Alternative

The original firmware targeting the **ESP32 WROOM** dev board. Adds a physical button on GPIO 4 for:
- **Short press**: Wake OLED and reset auto-sleep timer
- **Long press** (2 s+): Toggle always-on mode

- I2C pins: `GPIO 21` = SDA, `GPIO 22` = SCL
- Button: GPIO 4 → GND (INPUT_PULLUP)
- Supports: `debug` and `production` PlatformIO environments

---

### SlaveBox\_XiaoC3\_Mock — ESP32-C3 Mock Firmware

A **complete mock firmware** for the **Seeed Studio XIAO ESP32-C3** that emulates the full behaviour of the real firmware without needing physical I2C sensors attached.

**How it works:**
- `MockSensorManager` generates realistic, organically-drifting sensor values using sinusoidal math + noise — no I2C hardware required
- `BLEHelper` is a stub: `sendMap()` logs identical JSON payloads to Serial instead of BLE-notifying (easy to verify with Python bridge)
- `ScreenHelper` / `DisplayHelper` are the **real** SSD1306 implementations — plug in an OLED and it works
- `main.cpp` is **byte-for-byte identical** to the real S3 firmware — swap in real helpers to convert to production

**Mock sensor behaviour:**
| Sensor | Base Value | Variation |
|--------|-----------|-----------|
| BME280 temperature | 22 °C | ±1.5 °C slow drift + ±0.2 °C noise |
| BME280 humidity | 55 % | ±5 % slow drift |
| BME280 pressure | 1013.25 hPa | ±2 hPa slow drift |
| SGP30 eCO2 | 400 ppm (warm-up 15 s) | 400–900 ppm after warm-up |
| SGP30 TVOC | 0 ppb (warm-up) | 10–70 ppb after warm-up |
| BH1750 light | 350 lux | ±120 lux cloud drift + fluorescent ripple |

---

## 📁 Project Structure

```
SlaveBoxCode/
├── SlaveBox_XiaoS3/           # Primary — Seeed XIAO ESP32S3
│   ├── include/               # Header files
│   ├── src/                   # main.cpp, sensor helpers, BLE, display
│   └── platformio.ini         # debug + production environments
│
├── SlaveBox/                  # Alternative — ESP32 WROOM
│   ├── include/               # Header files (incl. ButtonHandler, ScreenPowerManager)
│   ├── src/                   # main.cpp, sensor helpers, BLE, display, button
│   └── platformio.ini         # debug + production environments
│
├── SlaveBox_XiaoC3_Mock/      # Mock firmware — Seeed XIAO ESP32-C3
│   ├── include/               # Header files (mock + real APIs)
│   ├── src/                   # MockSensorManager, real ScreenHelper, BLE stub
│   └── platformio.ini         # Single environment (seeed_xiao_esp32c3)
│
├── requirements.txt           # Python dependencies for BLE bridge
└── README.md
```

---

## 🚀 Getting Started

### Prerequisites

**Hardware:**
- ESP32 Development Board (XIAO ESP32S3 recommended, or WROOM)
- SSD1306 OLED Display (128x32, I2C)
- Supported I2C sensors (BME280, SGP30, BH1750)
- Push button (optional, WROOM only)
- jumper wires

**Software:**
- [PlatformIO IDE](https://platformio.org/) or [PlatformIO CLI](https://docs.platformio.org/en/latest/core/installation.html)
- USB cable for programming

### Dependencies (auto-installed via PlatformIO)

| Library | Version | Purpose |
|---------|---------|---------|
| Adafruit BME280 | ^2.3.0 | Temperature/Humidity/Pressure sensor |
| Adafruit SGP30 | ^2.0.3 | Air quality sensor |
| BH1750 (claws) | ^1.3.0 | Light sensor |
| ArduinoJson | ^7.2.1 | JSON serialization for BLE data |
| Adafruit GFX | ^1.12.4 | Graphics library for OLED |
| Adafruit SSD1306 | ^2.5.16 | OLED display driver |

### Wiring

#### Seeed Studio XIAO ESP32S3 Pinout (Primary)

| XIAO S3 Pin | Function | Connection |
|-------------|----------|------------|
| **D4 (GPIO5)** | SDA | SDA (all I2C devices) |
| **D5 (GPIO6)** | SCL | SCL (all I2C devices) |
| **3V3** | VCC | VCC (all sensors & display) |
| **GND** | GND | GND (all devices) |

#### ESP32 WROOM Pinout (Alternative)

| ESP32 Pin | Function | Connection |
|-----------|----------|------------|
| **GPIO 21** | SDA | SDA (all I2C devices) |
| **GPIO 22** | SCL | SCL (all I2C devices) |
| **GPIO 4** | GPIO | Button (other side to GND) |
| **3.3V** | VCC | VCC (all sensors & display) |
| **GND** | GND | GND (all devices) |

> **Note**: Multiple I2C devices share the same SDA/SCL bus. All sensors and the OLED connect in parallel.

### Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/JetsGPT/SlaveBoxCode.git
   cd SlaveBoxCode
   ```

2. **Open the desired project folder** (`SlaveBox_XiaoS3`, `SlaveBox`, or `SlaveBox_XiaoC3_Mock`) in PlatformIO.

3. **Build and upload:**
   ```bash
   # Debug mode (default — fast loop, verbose output)
   pio run -e debug -t upload

   # Production mode (power-saving — 5-minute sleep interval)
   pio run -e production -t upload
   ```

4. **Monitor serial output:**
   ```bash
   pio device monitor
   ```

---

## 🐛 Debug Mode vs Production Mode

Both real projects (`SlaveBox` and `SlaveBox_XiaoS3`) support two compile-time build modes controlled by the `DEBUG_MODE` preprocessor flag.

### Selecting a mode

```bash
# Debug mode (development)
pio run -e debug -t upload

# Production mode (deployment / battery-powered use)
pio run -e production -t upload
```

### Comparison

| Feature | Debug Mode (`DEBUG_MODE=1`) | Production Mode (`DEBUG_MODE=0`) |
|---------|-----------------------------|---------------------------------|
| **Sensor read interval** | Every loop iteration (~2–3 s) | Every **5 minutes** |
| **Serial output** | Full verbose scan log | Silent (no output) |
| **OLED display** | Active, cycles through metrics | Off (WROOM: button-wake only) |
| **BLE transmission** | Every read | Every read (if connected) |
| **Sleep** | None | Light sleep between reads |
| **Estimated power** | ~80 mA active | ~0.8 mA (light sleep avg.) |

### How it works

In **production mode**, after each sensor read and BLE transmission the device calls:

```cpp
esp_sleep_enable_timer_wakeup(5ULL * 60ULL * 1000000ULL); // 5 minutes
esp_light_sleep_start();
```

**Why light sleep instead of deep sleep?**
Light sleep preserves the BLE connection context and bonding keys in RAM. Deep sleep would reset the BLE stack and force re-pairing on every wake. Light sleep draws ~0.8 mA vs ~80 mA active — a 99% reduction in average current over a 5-minute cycle.

### Adding power-save to your build flags

The `DEBUG_MODE` flag is set exclusively in `platformio.ini` — no source file changes needed:

```ini
[env:production]
build_flags =
    -DDEBUG_MODE=0
```

---

## 💻 Usage

### Basic Example

The main loop automatically scans and reads all sensors and transmits data over BLE:

```cpp
#include <Arduino.h>
#include "runSetup.h"
#include "SensorManager.h"
#include "BLEHelper.h"
#include "DisplayHelper.h"

void setup() {
  runSetup();
  initializeSensors();
  delay(1000);
}

void loop() {
  std::map<String, std::map<String, float>> sensorData = scanAndReadAllSensors(true);

  if (!bleHelper.isPairing()) {
    if (sensorData.empty()) {
      displayNoSensors();
    } else {
      displaySensorData(sensorData);
    }
  }

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

### SensorManager Functions

#### `void initializeSensors()`
Initializes the I2C bus and prepares sensors for reading.

#### `std::map<String, std::map<String, float>> scanAndReadAllSensors(bool debug = true)`
Scans all known sensors and returns their readings.

**Parameters:**
- `debug` — Enable/disable serial output (default: `true`)

**Returns:** Nested map `{SensorName: {ValueType: Value}}`

#### `SensorInfo* getSensorByAddress(uint8_t addr)`
Retrieves sensor information by I2C address.

```cpp
SensorInfo* sensor = getSensorByAddress(0x77); // Get BME280 info
```

### Individual Sensor Helpers

#### BME280
```cpp
std::map<String, float> getValues_BME280(uint8_t addr, bool debug);
// Returns: {"temperature", "humidity", "pressure"}
```

#### SGP30
```cpp
std::map<String, float> getValues_SGP30(bool debug);
// Returns: {"eCO2", "TVOC"}
```

#### BH1750
```cpp
std::map<String, float> getValues_BH1750(uint8_t addr, bool debug);
// Returns: {"light"}
```

---

## 🔘 Button Controls & Power Management

> *(WROOM variant only — `SlaveBox/`)*

### Button Wiring

Connect a momentary push button between **GPIO 4** and **GND**. The internal pull-up resistor is enabled automatically.

### Button Functions

| Action | Function |
|--------|----------|
| **Short Press** | Wake the display and reset the auto-sleep timer |
| **Long Press** (2+ seconds) | Toggle "Always On" mode (disables auto-sleep) |

### Screen Power Management

| State | Behaviour |
|-------|-----------|
| **Default** | Screen starts OFF |
| **Auto-Sleep** | Screen off after 10 s of inactivity |
| **Wake on Pairing** | Screen auto-wakes to show BLE pairing PIN |
| **Always-On Mode** | Disables auto-sleep (toggled via long press) |

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
screenPowerManager.begin();          // Initialize (screen starts OFF)
screenPowerManager.update();         // Call every loop (checks timeout)
screenPowerManager.wake();           // Turn on and reset timer
screenPowerManager.sleep();          // Turn off immediately
screenPowerManager.toggleAlwaysOn(); // Toggle always-on mode

if (screenPowerManager.isScreenOn()) { ... }
if (screenPowerManager.isAlwaysOn()) { ... }
```

---

## 📡 BLE Communication

SlaveBox includes a BLE server for wireless data transmission.

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
    bleHelper.sendMap(sensorData);
}

// Check if pairing is in progress
if (bleHelper.isPairing()) {
    // PIN is being displayed on screen
}
```

### Secure Pairing

1. A 6-digit PIN is generated and displayed on the OLED screen
2. A 30-second countdown timer shows remaining time
3. User enters the PIN on their phone/device
4. Screen shows "SUCCESS!" or "FAILED!" based on result

---

## 🖥️ OLED Display

The 128x32 SSD1306 OLED display provides visual feedback.

### Screen Functions

```cpp
bool initScreen();                                          // Initialize display
updateScreen("Temperature", "24.5 C", true);               // Header + value
updateScreenWithProgress("ENTER PIN (25s)", "123-456", 83); // With countdown arc
clearScreen();                                              // Clear display
setDisplayPower(true);                                      // Power on
setDisplayPower(false);                                     // Power off
```

### DisplayHelper Functions

```cpp
displaySensorData(sensorData, 2000);  // Cycle metrics, 2 s each
displayNoSensors();                   // Show "No Sensors" message
```

### Display Modes

| Mode | Description |
|------|-------------|
| Header + Value | Title with divider line and large value text |
| Value Only | Large centered text (`showHeader = false`) |
| Progress Mode | Header, value, and circular countdown arc |

---

## 🐍 Python BLE Bridge

A Python-based BLE to MQTT bridge is available for integrating with home automation systems.

### Python Dependencies

```bash
pip install -r requirements.txt
```

**requirements.txt:**
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

**include/NewSensorHelper.h:**
```cpp
#ifndef NEWSENSORHELPER_H
#define NEWSENSORHELPER_H

#include <Arduino.h>
#include <map>

std::map<String, float> getValues_NewSensor(uint8_t addr, bool debug);

#endif
```

**src/NewSensorHelper.cpp:**
```cpp
#include "NewSensorHelper.h"
#include <Wire.h>

static bool initialized = false;

std::map<String, float> getValues_NewSensor(uint8_t addr, bool debug) {
    if (!initialized) {
        // Initialize sensor
        initialized = true;
    }

    std::map<String, float> values;
    values["measurement"] = sensorValue;
    return values;
}
```

### 2. Update SensorManager.cpp

Add include:
```cpp
#include "NewSensorHelper.h"
```

Add wrapper:
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
Your sensor will now be automatically scanned and read every loop.

---

## ❓ Troubleshooting

### Common Issues

| Problem | Solution |
|---------|----------|
| **No sensors detected** | Check I2C wiring (SDA/SCL). Verify 3.3V power. |
| **OLED not working** | Confirm I2C address is `0x3C`. Check for loose connections. |
| **BLE not advertising** | Ensure no other device is connected. Restart the ESP32. |
| **BLE pairing fails** | Enter PIN within 30 seconds. Ensure phone Bluetooth is on. |
| **Button not responding** | Verify button is between GPIO 4 and GND. *(WROOM only)* |
| **Display stays off** | Press button to wake. Long press to disable auto-sleep. *(WROOM only)* |
| **Sensor readings are wrong** | Allow sensors to warm up (SGP30 needs 15 seconds). |
| **Production mode not sleeping** | Ensure you uploaded with `pio run -e production -t upload`. |

### I2C Scanner

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

Enable verbose output to see detailed sensor readings:

```cpp
auto data = scanAndReadAllSensors(true);  // Verbose output
auto data = scanAndReadAllSensors(false); // Silent mode
```

Or build with `-DDEBUG_MODE=1` for full development experience.

---

## 📖 Glossary

| Term | Definition |
|------|------------|
| **I2C** | Inter-Integrated Circuit — two-wire serial communication protocol |
| **BLE** | Bluetooth Low Energy — wireless protocol optimized for low power |
| **SDA** | Serial Data Line — I2C data wire |
| **SCL** | Serial Clock Line — I2C clock wire |
| **eCO2** | Equivalent Carbon Dioxide — CO2 estimation based on VOC (ppm) |
| **TVOC** | Total Volatile Organic Compounds — air quality measure (ppb) |
| **lux** | Unit of illuminance (light intensity) |
| **hPa** | Hectopascal — atmospheric pressure unit (1 hPa = 1 mbar) |
| **UUID** | Universally Unique Identifier — identifies BLE services/characteristics |
| **OLED** | Organic Light-Emitting Diode — display technology |
| **Light Sleep** | ESP32 low-power mode that preserves RAM, BLE state, and bonding keys |
| **DEBUG_MODE** | Compile-time flag: `1` = fast dev loop, `0` = power-saving production |

---

## 🤝 Contributing

Contributions are welcome!

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

### Ideas for Contributions

- Support for additional sensors
- Web-based configuration interface
- MQTT direct publishing from ESP32
- Data logging to SD card
- Multi-room mesh networking

---

## 📜 License

This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.

---

## 👥 Authors

- **Julian** — *Initial work*

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
