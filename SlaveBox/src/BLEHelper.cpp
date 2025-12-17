#include "BLEHelper.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLESecurity.h> // [NEW] Security Header
#include <ArduinoJson.h>
// UUIDs matching the example provided
#define SERVICE_UUID        "cfa59c64-aeaf-42ac-bf8d-bc4a41ef5b0c"
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
    // --- [NEW] SECURITY IMPLEMENTATION START ---
    BLESecurity *pSecurity = new BLESecurity();
    
    // 1. Force LE Secure Connections (prevents passive sniffing)
    // ESP_LE_AUTH_REQ_SC_BOND: Secure Connections + Bonding (saving keys)
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
    // 2. Set IO Capability
    // ESP_IO_CAP_NONE tells the gateway "I have no display or buttons".
    // This triggers "Just Works" pairing (Encrypted, but no PIN entry).
    pSecurity->setCapability(ESP_IO_CAP_NONE); 
    // 3. Init keys
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    // --- [NEW] SECURITY IMPLEMENTATION END ---
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
        BLECharacteristic::PROPERTY_READ_ENC | BLECharacteristic::PROPERTY_NOTIFY
    );
    
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
        BLECharacteristic::PROPERTY_READ_ENC | BLECharacteristic::PROPERTY_NOTIFY
    );
    
    BLE2902* p2902Type = new BLE2902();
    p2902Type->setNotifications(true);
    p2902Type->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pimpl->pSensorTypeCharacteristic->addDescriptor(p2902Type);
    
    pimpl->pSensorTypeCharacteristic->setValue("unknown");
    pimpl->pBoxCharacteristic = pService->createCharacteristic(
        BOX_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ_ENC // [MODIFY] Encrypt box ID read too
    );
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
