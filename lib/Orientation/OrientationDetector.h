// OrientationDetector.h - Add constructor with address
#ifndef OrientationDetector_h
#define OrientationDetector_h

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>

enum Orientation {
    ORIENTATION_UNKNOWN,
    ORIENTATION_RIGHT_UP,
    ORIENTATION_DOWN
};

class OrientationDetector {
private:
    Adafruit_MPU6050 _mpu;
    int _address;
    Orientation _currentOrientation = ORIENTATION_UNKNOWN;
    Orientation _lastStableOrientation = ORIENTATION_UNKNOWN;
    unsigned long _lastChangeTime = 0;
    unsigned long _hysteresisMs = 50;
    float _threshold = 0.5;

public:
    // Constructor with address
    OrientationDetector(int address = 0x68) : _address(address) {}

    bool begin() {
        Wire.begin(MPU6050_SDA_PIN, MPU6050_SCL_PIN);
        return _mpu.begin(_address, &Wire);
    }

    void update() {
        sensors_event_t a, g, temp;
        _mpu.getEvent(&a, &g, &temp);
        
        Orientation newOrient = determineOrientation(a.acceleration.x, a.acceleration.y);
        
        if (newOrient != _currentOrientation) {
            if (millis() - _lastChangeTime > _hysteresisMs) {
                if (newOrient != ORIENTATION_UNKNOWN) {
                    _lastStableOrientation = newOrient;
                }
                _currentOrientation = newOrient;
                _lastChangeTime = millis();
            }
        }
    }

    Orientation determineOrientation(float ax, float ay) {
        if (abs(ax) < _threshold && abs(ay) > 0.7) {
            return ORIENTATION_RIGHT_UP;
        } else if (abs(ax) < _threshold && abs(ay) < -0.7) {
            return ORIENTATION_DOWN;
        }
        return ORIENTATION_UNKNOWN;
    }

    Orientation getStableOrientation() {
        if (_currentOrientation != ORIENTATION_UNKNOWN) {
            return _currentOrientation;
        }
        return _lastStableOrientation;
    }

    Orientation getLastStable() {
        return _lastStableOrientation;
    }
};

#endif