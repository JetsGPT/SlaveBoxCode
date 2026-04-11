#include "BLEHelper.h"
#include "ScreenHelper.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLESecurity.h>
#include <ArduinoJson.h>

// UUIDs matching the example provided
#define SERVICE_UUID        "cfa59c64-aeaf-42ac-bf8d-bc4a41ef5b0c"

// --- Pairing Timer Variables ---
#define PAIRING_TIMEOUT_SECONDS 30
#define PAIRING_COOLDOWN_MS 5000  // Rate limit: 5 seconds between pairing attempts
static volatile bool pairingInProgress = false;
static volatile uint32_t currentPassKey = 0;
static TaskHandle_t pairingTimerTaskHandle = NULL;
static unsigned long lastPairingAttempt = 0;

// Forward declaration
void pairingTimerTask(void* parameter);

void startPairingTimer(uint32_t passKey) {
    // Rate limiting check to prevent pairing DoS attacks
    unsigned long now = millis();
    if (pairingInProgress) {
        Serial.println("⚠️ Pairing already in progress, ignoring request");
        return;
    }
    if ((now - lastPairingAttempt) < PAIRING_COOLDOWN_MS && lastPairingAttempt != 0) {
        Serial.println("⚠️ Pairing rate limited, try again later");
        return;
    }
    lastPairingAttempt = now;
    
    currentPassKey = passKey;
    pairingInProgress = true;
    
    // Wake the screen and ensure display is on to show PIN
    setDisplayPower(true);
    
    // Create timer task if not already running
    if (pairingTimerTaskHandle == NULL) {
        xTaskCreate(
            pairingTimerTask,
            "PairingTimer",
            2048,
            NULL,
            1,
            &pairingTimerTaskHandle
        );
    }
}

void stopPairingTimer() {
    pairingInProgress = false;
    if (pairingTimerTaskHandle != NULL) {
        vTaskDelete(pairingTimerTaskHandle);
        pairingTimerTaskHandle = NULL;
    }
}

void pairingTimerTask(void* parameter) {
    int totalFrames = PAIRING_TIMEOUT_SECONDS * 4;  // 4 frames per second
    int remainingFrames = totalFrames;
    char pinStr[8];
    char displayStr[12];  // Increased buffer size for safety
    char headerStr[24];   // Increased buffer size for safety
    
    // Format PIN as XXX-XXX using snprintf for buffer safety
    snprintf(pinStr, sizeof(pinStr), "%06lu", (unsigned long)(currentPassKey % 1000000));
    snprintf(displayStr, sizeof(displayStr), "%.3s-%.3s", pinStr, pinStr + 3);
    
    while (pairingInProgress && remainingFrames > 0) {
        // Calculate percentage remaining (100 = full, 0 = empty)
        int percentage = (remainingFrames * 100) / totalFrames;
        int remainingSeconds = (remainingFrames + 3) / 4;  // Round up to nearest second
        
        // Format header with countdown using snprintf for buffer safety
        snprintf(headerStr, sizeof(headerStr), "ENTER PIN (%ds)", remainingSeconds);
        
        // Update screen with PIN and circular progress
        updateScreenWithProgress(String(headerStr), String(displayStr), percentage);
        
        remainingFrames--;
        vTaskDelay(250 / portTICK_PERIOD_MS);  // 250ms = 4 updates per second
    }
    
    // If we timed out (not stopped by successful/failed pairing)
    if (pairingInProgress && remainingFrames <= 0) {
        pairingInProgress = false;
        updateScreen("PAIRING", "TIMEOUT!", true);
        Serial.println("⏱️ Pairing Timeout");
    }
    
    pairingTimerTaskHandle = NULL;
    vTaskDelete(NULL);
}

// --- Security Callback Class ---
// This class handles the pairing events and displays the passkey on the screen
class MySecurity : public BLESecurityCallbacks {
    uint32_t onPassKeyRequest() {
        Serial.println("PassKey Request");
        return 123456; 
    }

    // The stack generates a random 6-digit code and sends it here
    // We display it on the screen so the user can type it into their phone
    void onPassKeyNotify(uint32_t pass_key) {
        Serial.println("========================================");
        Serial.println("       BLE PAIRING REQUEST              ");
        Serial.println("========================================");
        Serial.printf("    ENTER THIS PIN:  %06d             \n", pass_key);
        Serial.println("========================================");
        
        // Start the countdown timer task
        startPairingTimer(pass_key);
    }

    bool onConfirmPIN(uint32_t pass_key) {
        Serial.printf("The passkey value being compared is %d\n", pass_key);
        return true; 
    }

    bool onSecurityRequest() {
        Serial.println("Security Request Received");
        return true; 
    }

    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) {
        // Stop the timer first
        stopPairingTimer();
        
        if (cmpl.success) {
            Serial.println("✅ Pairing Successful");
            updateScreen("PAIRING", "SUCCESS!", true);
        } else {
            Serial.println("❌ Pairing Failed");
            updateScreen("PAIRING", "FAILED!", true);
        }
    }
};
#define SENSOR_CHAR_UUID    "49c92b70-42f5-49c3-bc38-5fe05b3df8e0"
#define SENSOR_TYPE_CHAR_UUID "3bee5811-4c6c-449a-b368-0b1391c6c1dc"
#define BOX_CHAR_UUID       "9d62dc0c-b4ef-40c4-9383-15bdc16870de"
// Implementation hidden to keep header light
class BLEHelper::Impl {
public:
    BLEServer* pServer = nullptr;
    BLECharacteristic* pSensorCharacteristic = nullptr;
    BLECharacteristic* pSensorTypeCharacteristic = nullptr;
    BLEDescriptor* pSensorTypeDescriptor = nullptr; // for clients that read descriptor
    BLECharacteristic* pBoxCharacteristic = nullptr;
    bool deviceConnected = false;
    String boxId = "";
    String lastSensorType = "unknown"; // cached primary type to present at connect
    class ServerCallbacks : public BLEServerCallbacks {
    public:
        ServerCallbacks(Impl *i): impl(i) {}
        void onConnect(BLEServer* pServer) override {
            impl->deviceConnected = true;
            Serial.println("Client connected");
            // When a client connects, ensure the sensor-type characteristic
            // contains a meaningful primary type (so the client read_gatt_char
            // call in the Python bridge returns a useful value).
            if (impl->pSensorTypeCharacteristic) {
                impl->pSensorTypeCharacteristic->setValue(impl->lastSensorType.c_str());
                // Do not notify here; the client will read the characteristic.
            }
            // Also update descriptor if present
            if (impl->pSensorTypeDescriptor) {
                impl->pSensorTypeDescriptor->setValue(impl->lastSensorType.c_str());
            }
        }
        void onDisconnect(BLEServer* pServer) override {
            impl->deviceConnected = false;
            Serial.println("Client disconnected");
            delay(500);
            BLEDevice::startAdvertising();
            Serial.println("Advertising restarted");
        }
    private:
        Impl *impl;
    };
};
BLEHelper::BLEHelper() {
    pimpl = new Impl();
}
BLEHelper bleHelper; // global instance
void BLEHelper::begin(const String &deviceName, const String &boxId) {
    pimpl->boxId = boxId;
    Serial.print("Initializing BLE device: "); Serial.println(deviceName);
    BLEDevice::init(deviceName.c_str());

    // --- SECURITY CONFIGURATION ---
    BLESecurity *pSecurity = new BLESecurity();
    
    // 1. Set Encryption Level: 
    //    MITM = Man In The Middle protection (Required to force Passkey entry)
    //    BOND = Save keys so we don't have to pair every time
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);

    // 2. Set IO Capability to DISPLAY ONLY
    //    This tells the phone: "I have a screen, I will show the user the code, 
    //    ask the user to type it into the phone."
    pSecurity->setCapability(ESP_IO_CAP_OUT); 

    // 3. Set Init Keys
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    
    // 4. Set Responder Keys (helps with compatibility)
    pSecurity->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    
    // 5. Register the security callbacks via BLEDevice
    BLEDevice::setSecurityCallbacks(new MySecurity());

    Serial.println("Security Configured: Passkey Pairing Enabled");
    // ------------------------------------

    Serial.println("========================================");
    Serial.print("ESP32 MAC Address: ");
    Serial.println(BLEDevice::getAddress().toString().c_str());
    Serial.println("========================================");
    pimpl->pServer = BLEDevice::createServer();
    pimpl->pServer->setCallbacks(new BLEHelper::Impl::ServerCallbacks(pimpl));
    BLEService *pService = pimpl->pServer->createService(SERVICE_UUID);
    // [MODIFY] Enforce Encryption (PROPERTY_READ_ENC)
    pimpl->pSensorCharacteristic = pService->createCharacteristic(
        SENSOR_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pimpl->pSensorCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED);
    
    // [MODIFY] Secure the CCCD descriptor
    BLE2902* p2902Sensor = new BLE2902();
    p2902Sensor->setNotifications(true);
    p2902Sensor->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pimpl->pSensorCharacteristic->addDescriptor(p2902Sensor);
    // Also add a descriptor with the SENSOR_TYPE UUID to support clients that
    // expect the sensor type as a descriptor attached to the sensor value
    // characteristic (legacy behavior).
    pimpl->pSensorTypeDescriptor = new BLEDescriptor(SENSOR_TYPE_CHAR_UUID);
    pimpl->pSensorTypeDescriptor->setValue("unknown");
    pimpl->pSensorTypeDescriptor->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED); // [NEW] Secure descriptor
    pimpl->pSensorCharacteristic->addDescriptor(pimpl->pSensorTypeDescriptor);
    // Make sensor-type characteristic readable and notifiable
    // [MODIFY] Enforce Encryption (PROPERTY_READ_ENC)
    pimpl->pSensorTypeCharacteristic = pService->createCharacteristic(
        SENSOR_TYPE_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pimpl->pSensorTypeCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED);
    
    BLE2902* p2902Type = new BLE2902();
    p2902Type->setNotifications(true);
    p2902Type->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pimpl->pSensorTypeCharacteristic->addDescriptor(p2902Type);
    
    pimpl->pSensorTypeCharacteristic->setValue("unknown");
    pimpl->pBoxCharacteristic = pService->createCharacteristic(
        BOX_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ // [MODIFY] Encrypt box ID read too
    );
    pimpl->pBoxCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED);
    pimpl->pBoxCharacteristic->setValue(boxId.c_str());
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMaxPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("BLE Temperature Sensor is now advertising!");
    Serial.print("Service UUID: "); Serial.println(SERVICE_UUID);
    Serial.println("Waiting for client...");
}
bool BLEHelper::isConnected() {
    return pimpl->deviceConnected;
}

bool BLEHelper::isPairing() {
    return pairingInProgress;
}
void BLEHelper::sendMap(const std::map<String, std::map<String, float>> &data) {
    if (!pimpl->deviceConnected || pimpl->pSensorCharacteristic == nullptr) {
        // nothing to do if no client connected
        return;
    }
    // Iterate sensors and metrics. For each metric send:
    // - SENSOR_TYPE_CHAR_UUID: sensor type (e.g., "temperature")
    // - SENSOR_CHAR_UUID: JSON format {"type": "temperature", "value": 23.5}
    // Log the entire incoming map to the Serial monitor for debugging
    Serial.println("--- Sending sensor map over BLE ---");
    for (const auto &sensorPair : data) {
        const String &sensorName = sensorPair.first;
        const std::map<String, float> &metrics = sensorPair.second;
        for (const auto &metricPair : metrics) {
            const String &metricName = metricPair.first;
            float value = metricPair.second;
            // Print a readable line to Serial before sending so the user
            // can inspect what will be sent over BLE.
            // Example: "bh1750/light: 123.4500"
            Serial.print(sensorName);
            Serial.print("/");
            Serial.print(metricName);
            Serial.print(": ");
            Serial.println(value, 4);
            // Update the sensor type characteristic with the current metric name
            // This allows the Python script to know what type of data is coming
            pimpl->pSensorTypeCharacteristic->setValue(metricName.c_str());
            pimpl->pSensorTypeCharacteristic->notify();
            
            // Update cached sensor type
            pimpl->lastSensorType = metricName;
            if (pimpl->pSensorTypeDescriptor) {
                pimpl->pSensorTypeDescriptor->setValue(metricName.c_str());
            }
            // small spacing to ensure client processes the type notification first
            delay(20);
            // Create JSON payload: {"type": "temperature", "value": 23.5}
            JsonDocument doc;
            doc["type"] = metricName;
            doc["value"] = value;
            
            String jsonString;
            serializeJson(doc, jsonString);
            // Send JSON string as sensor value
            pimpl->pSensorCharacteristic->setValue(jsonString.c_str());
            pimpl->pSensorCharacteristic->notify();
            Serial.print("  -> Sent JSON: ");
            Serial.println(jsonString);
            // small spacing to avoid flooding the BLE stack
            delay(60);
        }
    }
}
