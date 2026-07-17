#ifndef BatteryManager_h
#define BatteryManager_h

#include <Config.h>

class BatteryManager {
private:
    float _voltage = 0;
    int _percentage = 0;
    bool _isCharging = false;
    bool _isLow = false;
    unsigned long _lastRead = 0;
    unsigned long _readInterval = 5000;  // Read every 5 seconds
    
    // Battery calibration constants (adjust based on your battery)
    const float VOLTAGE_MIN = 3.0;   // 0% battery
    const float VOLTAGE_MAX = 4.2;   // 100% battery
    
public:
    void begin() {
        pinMode(BATTERY_ADC_PIN, INPUT);
        Serial.println("✅ Battery Manager initialized");
        readBattery();
    }
    
    void update() {
        if (millis() - _lastRead > _readInterval) {
            readBattery();
        }
    }
    
    void readBattery() {
        // Read ADC value (0-4095 for ESP32)
        int adcValue = analogRead(BATTERY_ADC_PIN);
        
        // Convert to voltage (3.3V reference, voltage divider 2:1)
        _voltage = (adcValue / 4095.0) * 3.3 * 2.0;
        
        // Calculate percentage
        _percentage = mapFloat(_voltage, VOLTAGE_MIN, VOLTAGE_MAX, 0, 100);
        _percentage = constrain(_percentage, 0, 100);
        
        // Check low battery
        _isLow = (_percentage < 20);
        
        _lastRead = millis();
        
        // Debug output (every 10 reads)
        static int readCount = 0;
        if (++readCount % 10 == 0) {
            Serial.printf("🔋 Battery: %.2fV, %d%%\n", _voltage, _percentage);
        }
    }
    
    float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
    
    // ===== Getters =====
    float getVoltage() { return _voltage; }
    int getPercentage() { return _percentage; }
    bool isLow() { return _isLow; }
    bool isCharging() { 
        // Detect charging by voltage increase while plugged in
        // Simple: if voltage > 4.0V, assume charging
        return _voltage > 4.0; 
    }
    
    // ===== Battery Life Estimation =====
    int getEstimatedHoursRemaining() {
        // Rough estimate: 400mAh battery
        // Idle: 30mA → ~13 hours
        // Recording: 80mA → ~5 hours
        // Playing: 120mA → ~3 hours
        if (_percentage < 5) return 0;
        return (_percentage * 400) / 80;  // Assuming recording usage
    }
    
    // ===== Charging Status =====
    bool isChargingComplete() {
        return _voltage > 4.15 && isCharging();
    }
    
    String getStatusString() {
        String status = "Battery: ";
        status += String(_percentage) + "% (";
        status += String(_voltage, 2) + "V)";
        
        if (isCharging()) {
            status += " ⚡ Charging";
        } else if (_isLow) {
            status += " ⚠️ LOW!";
        }
        
        return status;
    }
};

#endif