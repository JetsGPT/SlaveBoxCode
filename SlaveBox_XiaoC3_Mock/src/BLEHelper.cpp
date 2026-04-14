// ============================================================
// BLEHelper.cpp — XIAO ESP32-C3 Mock
//
// Full BLE implementation transplanted from SlaveBox_XiaoS3.
// The ESP32-C3 uses the identical Arduino BLE stack (BLEDevice /
// BLEServer / BLESecurity) so this file compiles unchanged on C3.
//
// Sensor readings come from MockSensorManager, but they are
// broadcast over a real BLE connection with the same UUIDs,
// security settings, and JSON payload format as the production
// firmware — making this a true end-to-end integration test.
// ============================================================

#include "BLEHelper.h"
#include "ScreenHelper.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLESecurity.h>
#include <ArduinoJson.h>

// Service / characteristic UUIDs — identical to real firmware
#define SERVICE_UUID          "cfa59c64-aeaf-42ac-bf8d-bc4a41ef5b0c"
#define SENSOR_CHAR_UUID      "49c92b70-42f5-49c3-bc38-5fe05b3df8e0"
#define SENSOR_TYPE_CHAR_UUID "3bee5811-4c6c-449a-b368-0b1391c6c1dc"
#define BOX_CHAR_UUID         "9d62dc0c-b4ef-40c4-9383-15bdc16870de"

// --- Pairing timer ---
#define PAIRING_TIMEOUT_SECONDS 30
#define PAIRING_COOLDOWN_MS     5000

static volatile bool     pairingInProgress  = false;
static volatile uint32_t currentPassKey     = 0;
static TaskHandle_t      pairingTimerHandle = NULL;
static unsigned long     lastPairingAttempt = 0;

void pairingTimerTask(void* parameter);

void startPairingTimer(uint32_t passKey) {
    unsigned long now = millis();
    if (pairingInProgress) return;
    if (lastPairingAttempt != 0 && (now - lastPairingAttempt) < PAIRING_COOLDOWN_MS) return;
    lastPairingAttempt = now;
    currentPassKey     = passKey;
    pairingInProgress  = true;
    setDisplayPower(true);
    if (pairingTimerHandle == NULL) {
        xTaskCreate(pairingTimerTask, "PairingTimer", 2048, NULL, 1, &pairingTimerHandle);
    }
}

void stopPairingTimer() {
    pairingInProgress = false;
    if (pairingTimerHandle != NULL) {
        vTaskDelete(pairingTimerHandle);
        pairingTimerHandle = NULL;
    }
}

void pairingTimerTask(void* parameter) {
    int   totalFrames     = PAIRING_TIMEOUT_SECONDS * 4;
    int   remainingFrames = totalFrames;
    char  pinStr[8], displayStr[12], headerStr[24];

    snprintf(pinStr,     sizeof(pinStr),     "%06lu", (unsigned long)(currentPassKey % 1000000));
    snprintf(displayStr, sizeof(displayStr), "%.3s-%.3s", pinStr, pinStr + 3);

    while (pairingInProgress && remainingFrames > 0) {
        int percentage       = (remainingFrames * 100) / totalFrames;
        int remainingSeconds = (remainingFrames + 3) / 4;
        snprintf(headerStr, sizeof(headerStr), "ENTER PIN (%ds)", remainingSeconds);
        updateScreenWithProgress(String(headerStr), String(displayStr), percentage);
        remainingFrames--;
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
    if (pairingInProgress && remainingFrames <= 0) {
        pairingInProgress = false;
        updateScreen("PAIRING", "TIMEOUT!", true);
    }
    pairingTimerHandle = NULL;
    vTaskDelete(NULL);
}

// --- Security callbacks ---
class MockSecurity : public BLESecurityCallbacks {
    uint32_t onPassKeyRequest() override { return 123456; }

    void onPassKeyNotify(uint32_t pass_key) override {
        // Print ONLY the PIN — nothing else
        Serial.printf("%06d\n", pass_key);
        startPairingTimer(pass_key);
    }

    bool onConfirmPIN(uint32_t pass_key) override { return true; }
    bool onSecurityRequest() override { return true; }

    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) override {
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

// --- Pimpl implementation ---
class BLEHelper::Impl {
public:
    BLEServer*         pServer                = nullptr;
    BLECharacteristic* pSensorChar            = nullptr;
    BLECharacteristic* pSensorTypeChar        = nullptr;
    BLEDescriptor*     pSensorTypeDescriptor  = nullptr;
    BLECharacteristic* pBoxChar               = nullptr;
    bool               deviceConnected        = false;
    String             lastSensorType         = "unknown";

    class ServerCallbacks : public BLEServerCallbacks {
    public:
        explicit ServerCallbacks(Impl* i) : impl(i) {}
        void onConnect(BLEServer*) override {
            impl->deviceConnected = true;
            Serial.println("[C3 MOCK BLE] Client connected");
            if (impl->pSensorTypeChar)
                impl->pSensorTypeChar->setValue(impl->lastSensorType.c_str());
            if (impl->pSensorTypeDescriptor)
                impl->pSensorTypeDescriptor->setValue(impl->lastSensorType.c_str());
        }
        void onDisconnect(BLEServer*) override {
            impl->deviceConnected = false;
            Serial.println("[C3 MOCK BLE] Client disconnected — restarting advertising");
            delay(500);
            BLEDevice::startAdvertising();
        }
    private:
        Impl* impl;
    };
};

BLEHelper::BLEHelper() { pimpl = new Impl(); }

BLEHelper bleHelper;

void BLEHelper::begin(const String& deviceName, const String& boxId) {
    Serial.print("[C3 MOCK BLE] Starting BLE as: "); Serial.println(deviceName);

    BLEDevice::init(deviceName.c_str());

    // Security (passkey pairing — same as real firmware)
    BLESecurity* pSec = new BLESecurity();
    pSec->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_MITM_BOND);
    pSec->setCapability(ESP_IO_CAP_OUT);
    pSec->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    pSec->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    BLEDevice::setSecurityCallbacks(new MockSecurity());

    Serial.print("[C3 MOCK BLE] MAC: ");
    Serial.println(BLEDevice::getAddress().toString().c_str());

    pimpl->pServer = BLEDevice::createServer();
    pimpl->pServer->setCallbacks(new BLEHelper::Impl::ServerCallbacks(pimpl));

    BLEService* pService = pimpl->pServer->createService(SERVICE_UUID);

    // Sensor data characteristic
    pimpl->pSensorChar = pService->createCharacteristic(
        SENSOR_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pimpl->pSensorChar->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED);
    BLE2902* p2902s = new BLE2902();
    p2902s->setNotifications(true);
    p2902s->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pimpl->pSensorChar->addDescriptor(p2902s);

    // Sensor-type descriptor (legacy support)
    pimpl->pSensorTypeDescriptor = new BLEDescriptor(SENSOR_TYPE_CHAR_UUID);
    pimpl->pSensorTypeDescriptor->setValue("unknown");
    pimpl->pSensorTypeDescriptor->setAccessPermissions(
        ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pimpl->pSensorChar->addDescriptor(pimpl->pSensorTypeDescriptor);

    // Sensor-type characteristic
    pimpl->pSensorTypeChar = pService->createCharacteristic(
        SENSOR_TYPE_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
    );
    pimpl->pSensorTypeChar->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED);
    BLE2902* p2902t = new BLE2902();
    p2902t->setNotifications(true);
    p2902t->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pimpl->pSensorTypeChar->addDescriptor(p2902t);
    pimpl->pSensorTypeChar->setValue("unknown");

    // Box ID characteristic
    pimpl->pBoxChar = pService->createCharacteristic(
        BOX_CHAR_UUID, BLECharacteristic::PROPERTY_READ);
    pimpl->pBoxChar->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED);
    pimpl->pBoxChar->setValue(boxId.c_str());

    pService->start();

    BLEAdvertising* pAdv = BLEDevice::getAdvertising();
    pAdv->addServiceUUID(SERVICE_UUID);
    pAdv->setScanResponse(true);
    pAdv->setMinPreferred(0x06);
    pAdv->setMaxPreferred(0x12);
    BLEDevice::startAdvertising();

    Serial.println("[C3 MOCK BLE] Advertising started — device is now visible in BLE scans ✅");
    Serial.print("[C3 MOCK BLE] Service UUID: "); Serial.println(SERVICE_UUID);
}

bool BLEHelper::isConnected() { return pimpl->deviceConnected; }
bool BLEHelper::isPairing()   { return pairingInProgress; }

void BLEHelper::sendMap(const std::map<String, std::map<String, float>>& data) {
    if (!pimpl->deviceConnected || pimpl->pSensorChar == nullptr) return;

    Serial.println("--- [C3 MOCK BLE] Sending sensor map ---");
    for (const auto& sensorPair : data) {
        const String& sensorName = sensorPair.first;
        for (const auto& metricPair : sensorPair.second) {
            const String& metricName = metricPair.first;
            float         value      = metricPair.second;

            Serial.print(sensorName); Serial.print("/");
            Serial.print(metricName); Serial.print(": ");
            Serial.println(value, 4);

            pimpl->pSensorTypeChar->setValue(metricName.c_str());
            pimpl->pSensorTypeChar->notify();
            pimpl->lastSensorType = metricName;
            if (pimpl->pSensorTypeDescriptor)
                pimpl->pSensorTypeDescriptor->setValue(metricName.c_str());

            delay(20);

            JsonDocument doc;
            doc["type"]  = metricName;
            doc["value"] = value;
            String json;
            serializeJson(doc, json);

            pimpl->pSensorChar->setValue(json.c_str());
            pimpl->pSensorChar->notify();
            Serial.print("  -> JSON: "); Serial.println(json);
            delay(60);
        }
    }
}
