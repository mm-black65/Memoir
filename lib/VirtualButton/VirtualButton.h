// VirtualButton.h - Non-blocking button detection
#ifndef VirtualButton_h
#define VirtualButton_h

#include <Arduino.h>

class VirtualButton {
public:
    enum Event {
        EVENT_NONE,
        EVENT_SINGLE_CLICK,
        EVENT_DOUBLE_CLICK,
        EVENT_LONG_PRESS
    };

private:
    int _pin;
    bool _lastState = false;
    bool _currentState = false;
    unsigned long _lastDebounceTime = 0;
    unsigned long _debounceDelay = 50;
    unsigned long _lastClickTime = 0;
    unsigned long _doubleClickWindow = 400;
    unsigned long _longPressTime = 2000;
    bool _singleClickPending = false;
    unsigned long _clickStartTime = 0;
    bool _isLongPress = false;

public:
    VirtualButton(int pin) : _pin(pin) {}

    void begin() {
        pinMode(_pin, INPUT_PULLUP);
        _lastState = digitalRead(_pin);
        _currentState = _lastState;
    }

    void update() {
        bool reading = digitalRead(_pin);
        
        if (reading != _lastState) {
            _lastDebounceTime = millis();
        }
        
        if ((millis() - _lastDebounceTime) > _debounceDelay) {
            _currentState = reading;
        }
        
        _lastState = reading;
    }

    bool isPressed() {
        return _currentState == HIGH;  // TTP223 outputs HIGH on touch
    }

    Event getEvent() {
        if (!isPressed()) {
            _isLongPress = false;
            return EVENT_NONE;
        }

        // Long press detection
        if (!_isLongPress && (millis() - _clickStartTime > _longPressTime)) {
            _isLongPress = true;
            _singleClickPending = false;
            return EVENT_LONG_PRESS;
        }

        if (!isPressed() && _clickStartTime > 0) {
            if (_isLongPress) {
                _clickStartTime = 0;
                return EVENT_NONE;
            }
            
            unsigned long pressDuration = millis() - _clickStartTime;
            _clickStartTime = 0;
            
            if (pressDuration > 100) {
                return EVENT_SINGLE_CLICK;
            }
        }

        return EVENT_NONE;
    }
};

#endif