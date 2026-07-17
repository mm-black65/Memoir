// BLEManager.h - BLE Communication with PWA
#ifndef BLEManager_h
#define BLEManager_h

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <ArduinoJson.h>

#define SERVICE_UUID        "0000fff0-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "0000fff1-0000-1000-8000-00805f9b34fb"

class BLEManager {
private:
    BLEServer* _server = nullptr;
    BLECharacteristic* _characteristic = nullptr;
    bool _connected = false;
    String _deviceName = "Memoir";
    
    // Callbacks
    void (*_onConnected)() = nullptr;
    void (*_onDisconnected)() = nullptr;
    
    class ServerCallbacks : public BLEServerCallbacks {
    private:
        BLEManager* _manager;
    public:
        ServerCallbacks(BLEManager* manager) : _manager(manager) {}
        
        void onConnect(BLEServer* pServer) override {
            _manager->_connected = true;
            if (_manager->_onConnected) _manager->_onConnected();
            Serial.println("📱 BLE connected");
        }
        
        void onDisconnect(BLEServer* pServer) override {
            _manager->_connected = false;
            if (_manager->_onDisconnected) _manager->_onDisconnected();
            Serial.println("📱 BLE disconnected");
            // Restart advertising
            pServer->getAdvertising()->start();
        }
    };
    
public:
    void begin(const char* name = "Memoir") {
        _deviceName = name;
        
        BLEDevice::init(_deviceName.c_str());
        _server = BLEDevice::createServer();
        _server->setCallbacks(new ServerCallbacks(this));
        
        BLEService* service = _server->createService(SERVICE_UUID);
        _characteristic = service->createCharacteristic(
            CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_NOTIFY
        );
        
        _characteristic->addDescriptor(new BLE2902());
        
        service->start();
        
        BLEAdvertising* advertising = _server->getAdvertising();
        advertising->addServiceUUID(SERVICE_UUID);
        advertising->start();
        
        Serial.printf("✅ BLE initialized as '%s'\n", _deviceName.c_str());
    }
    
    void sendNotification(JsonDocument& doc) {
        if (!_connected) return;
        
        String jsonString;
        serializeJson(doc, jsonString);
        
        _characteristic->setValue(jsonString.c_str());
        _characteristic->notify();
        
        Serial.printf("📤 BLE notification: %s\n", jsonString.c_str());
    }
    
    void sendNotification(const char* jsonString) {
        if (!_connected) return;
        
        _characteristic->setValue(jsonString);
        _characteristic->notify();
        
        Serial.printf("📤 BLE notification: %s\n", jsonString);
    }
    
    // Helper to send JSON with initializer list
    void sendNotification(std::initializer_list<std::pair<const char*, const char*>> pairs) {
        if (!_connected) return;
        
        DynamicJsonDocument doc(256);
        for (auto& pair : pairs) {
            doc[pair.first] = pair.second;
        }
        sendNotification(doc);
    }
    
    void sendNotification(std::initializer_list<std::pair<const char*, int>> pairs) {
        if (!_connected) return;
        
        DynamicJsonDocument doc(256);
        for (auto& pair : pairs) {
            doc[pair.first] = pair.second;
        }
        sendNotification(doc);
    }
    
    void sendNotification(std::initializer_list<std::pair<const char*, bool>> pairs) {
        if (!_connected) return;
        
        DynamicJsonDocument doc(256);
        for (auto& pair : pairs) {
            doc[pair.first] = pair.second;
        }
        sendNotification(doc);
    }
    
    bool isConnected() { return _connected; }
    
    void onConnected(void (*cb)()) { _onConnected = cb; }
    void onDisconnected(void (*cb)()) { _onDisconnected = cb; }
};

#endif