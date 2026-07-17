// lib/PowerManager/PowerManager.h
#ifndef PowerManager_h
#define PowerManager_h

#include <esp_sleep.h>
#include <esp_bt.h>
#include <esp_wifi.h>
#include <Config.h>

class PowerManager {
private:
    bool _isSleeping = false;
    bool _forceAwake = false;
    unsigned long _lastActivityTime = 0;
    unsigned long _inactivityTimeout = 5000;  // 5 seconds
    
public:
    void begin() {
        Serial.println("✅ PowerManager initialized");
        
        // Configure wake-up sources
        // TOUCH_PIN (GPIO0) is the only wake source
        esp_sleep_enable_ext0_wakeup((gpio_num_t)TOUCH_PIN, 1);  // HIGH level wakes
    }
    
    void update() {
        // Check if we should sleep
        if (shouldSleep()) {
            goToSleep();
        }
    }
    
    bool shouldSleep() {
        if (_forceAwake) return false;
        if (_isSleeping) return false;
        
        // Only sleep if no activity for timeout period
        return (millis() - _lastActivityTime > _inactivityTimeout);
    }
    
    void activity() {
        _lastActivityTime = millis();
        
        if (_isSleeping) {
            wakeUp();
        }
    }
    
    void goToSleep() {
        if (_isSleeping) return;
        
        _isSleeping = true;
        Serial.println("💤 Entering deep sleep...");
        Serial.flush();
        
        // Turn off WiFi and BLE to save power
        esp_wifi_stop();
        esp_bt_controller_disable();
        
        // Disable ADC to save power
        adc_power_off();
        
        // Enable wake on touch (GPIO0)
        esp_sleep_enable_ext0_wakeup((gpio_num_t)TOUCH_PIN, 1);
        
        // Enter deep sleep
        esp_deep_sleep_start();
        // Code will not reach here
    }
    
    void wakeUp() {
        if (!_isSleeping) return;
        
        _isSleeping = false;
        _lastActivityTime = millis();
        Serial.println("☀️ Woke up from touch!");
        
        // Re-initialize peripherals if needed
        // (Will be handled by main sketch)
    }
    
    void forceAwake(bool force) {
        _forceAwake = force;
        if (force) {
            _lastActivityTime = millis();
        }
    }
    
    bool isSleeping() { return _isSleeping; }
    
    void setInactivityTimeout(unsigned long ms) {
        _inactivityTimeout = ms;
    }
    
    unsigned long getInactivityTimeout() {
        return _inactivityTimeout;
    }
};

#endif