#ifndef LEDManager_h
#define LEDManager_h

#include <Adafruit_NeoPixel.h>
#include <Config.h>

class LEDManager {
private:
    Adafruit_NeoPixel* _pixel;
    
    // Color definitions
    const uint32_t COLOR_OFF     = 0x000000;
    const uint32_t COLOR_RED     = 0xFF0000;
    const uint32_t COLOR_GREEN   = 0x00FF00;
    const uint32_t COLOR_BLUE    = 0x0000FF;
    const uint32_t COLOR_WHITE   = 0xFFFFFF;
    const uint32_t COLOR_YELLOW  = 0xFFFF00;
    const uint32_t COLOR_ORANGE  = 0xFF8800;
    const uint32_t COLOR_PURPLE  = 0x8800FF;
    const uint32_t COLOR_CYAN    = 0x00FFFF;
    
    // State
    enum LEDState {
        LED_OFF,
        LED_SOLID,
        LED_BREATHING,
        LED_BLINKING,
        LED_FLASHING,
        LED_PULSING,
        LED_FADE_IN,
        LED_FADE_OUT
    };
    
    LEDState _state = LED_OFF;
    uint32_t _color = COLOR_OFF;
    uint32_t _secondaryColor = COLOR_OFF;
    unsigned long _lastUpdate = 0;
    int _brightness = 50;  // 0-255
    float _breathPhase = 0;
    
    // Pattern parameters
    unsigned long _blinkOnTime = 500;
    unsigned long _blinkOffTime = 500;
    unsigned long _patternStartTime = 0;
    bool _patternState = false;
    
public:
    LEDManager() {
        _pixel = new Adafruit_NeoPixel(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
    }
    
    void begin() {
        _pixel->begin();
        _pixel->setBrightness(_brightness);
        _pixel->show();
        Serial.println("✅ LED Manager initialized");
    }
    
    void update() {
        _lastUpdate = millis();
        
        switch (_state) {
            case LED_OFF:
                setColor(COLOR_OFF);
                break;
                
            case LED_SOLID:
                setColor(_color);
                break;
                
            case LED_BREATHING:
                breathingEffect();
                break;
                
            case LED_BLINKING:
                blinkingEffect();
                break;
                
            case LED_FLASHING:
                flashingEffect();
                break;
                
            case LED_PULSING:
                pulsingEffect();
                break;
                
            case LED_FADE_IN:
                fadeInEffect();
                break;
                
            case LED_FADE_OUT:
                fadeOutEffect();
                break;
        }
    }
    
    // ===== State Setters =====
    
    void setSolid(uint32_t color) {
        _state = LED_SOLID;
        _color = color;
        update();
    }
    
    void setBreathing(uint32_t color) {
        _state = LED_BREATHING;
        _color = color;
        _breathPhase = 0;
        update();
    }
    
    void setBlinking(uint32_t color, unsigned long onTime = 500, unsigned long offTime = 500) {
        _state = LED_BLINKING;
        _color = color;
        _blinkOnTime = onTime;
        _blinkOffTime = offTime;
        _patternStartTime = millis();
        _patternState = true;
        update();
    }
    
    void setFlashing(uint32_t color, unsigned long duration = 200) {
        _state = LED_FLASHING;
        _color = color;
        _blinkOnTime = duration;
        _blinkOffTime = duration;
        _patternStartTime = millis();
        _patternState = true;
        update();
    }
    
    void setPulsing(uint32_t color1, uint32_t color2) {
        _state = LED_PULSING;
        _color = color1;
        _secondaryColor = color2;
        _breathPhase = 0;
        update();
    }
    
    void fadeIn(uint32_t color, unsigned long duration = 1000) {
        _state = LED_FADE_IN;
        _color = color;
        _patternStartTime = millis();
        update();
    }
    
    void fadeOut(unsigned long duration = 500) {
        _state = LED_FADE_OUT;
        _patternStartTime = millis();
        update();
    }
    
    void off() {
        _state = LED_OFF;
        update();
    }
    
    // ===== Preset Patterns =====
    
    void ready() {
        setSolid(COLOR_GREEN);
    }
    
    void recording() {
        setBreathing(COLOR_RED);
    }
    
    void recordingReminder() {
        setFlashing(COLOR_RED, 100);
        // Will auto-revert after flash
    }
    
    void playback() {
        setSolid(COLOR_BLUE);
    }
    
    void transcribing() {
        setPulsing(COLOR_PURPLE, COLOR_CYAN);
    }
    
    void deleting() {
        setFlashing(COLOR_RED, 200);
    }
    
    void lowBattery() {
        setBlinking(COLOR_YELLOW, 1000, 2000);
    }
    
    void charging() {
        setSolid(COLOR_ORANGE);
    }
    
    void chargingComplete() {
        setSolid(COLOR_GREEN);
    }
    
    void error() {
        setFlashing(COLOR_RED, 200);
    }
    
    void wake() {
        setFlashing(COLOR_WHITE, 100);
    }
    
    void sleep() {
        off();
    }
    
    void startup() {
        // Fade from white to green
        fadeIn(COLOR_WHITE, 1000);
        // Will transition to green after fade
    }
    
    // ===== Effects Implementation =====
    
    void breathingEffect() {
        float brightness = (sin(_breathPhase) + 1.0) / 2.0;  // 0 to 1
        uint32_t color = scaleColor(_color, brightness);
        setColor(color);
        _breathPhase += 0.02;  // Speed of breathing
        if (_breathPhase > 2 * PI) _breathPhase = 0;
    }
    
    void blinkingEffect() {
        unsigned long elapsed = millis() - _patternStartTime;
        bool on = (elapsed % (_blinkOnTime + _blinkOffTime)) < _blinkOnTime;
        setColor(on ? _color : COLOR_OFF);
    }
    
    void flashingEffect() {
        // Flash on, then off (single flash)
        unsigned long elapsed = millis() - _patternStartTime;
        if (elapsed < _blinkOnTime) {
            setColor(_color);
        } else {
            setColor(COLOR_OFF);
            // Auto revert after flash
            if (elapsed > _blinkOnTime + _blinkOffTime) {
                // Revert to previous state (will be handled by caller)
            }
        }
    }
    
    void pulsingEffect() {
        float progress = (sin(_breathPhase) + 1.0) / 2.0;
        uint32_t color = interpolateColor(_color, _secondaryColor, progress);
        setColor(color);
        _breathPhase += 0.01;
        if (_breathPhase > 2 * PI) _breathPhase = 0;
    }
    
    void fadeInEffect() {
        unsigned long elapsed = millis() - _patternStartTime;
        float progress = constrain((float)elapsed / 1000.0, 0, 1);
        uint32_t color = scaleColor(_color, progress);
        setColor(color);
        
        if (progress >= 1.0) {
            _state = LED_SOLID;
            _color = color;
        }
    }
    
    void fadeOutEffect() {
        unsigned long elapsed = millis() - _patternStartTime;
        float progress = constrain((float)elapsed / 500.0, 0, 1);
        uint32_t color = scaleColor(_color, 1.0 - progress);
        setColor(color);
        
        if (progress >= 1.0) {
            _state = LED_OFF;
            setColor(COLOR_OFF);
        }
    }
    
    // ===== Helper Functions =====
    
    void setColor(uint32_t color) {
        _pixel->setPixelColor(0, color);
        _pixel->show();
    }
    
    uint32_t scaleColor(uint32_t color, float scale) {
        uint8_t r = (color >> 16) & 0xFF;
        uint8_t g = (color >> 8) & 0xFF;
        uint8_t b = color & 0xFF;
        
        r = (uint8_t)(r * scale);
        g = (uint8_t)(g * scale);
        b = (uint8_t)(b * scale);
        
        return (r << 16) | (g << 8) | b;
    }
    
    uint32_t interpolateColor(uint32_t color1, uint32_t color2, float t) {
        uint8_t r1 = (color1 >> 16) & 0xFF;
        uint8_t g1 = (color1 >> 8) & 0xFF;
        uint8_t b1 = color1 & 0xFF;
        
        uint8_t r2 = (color2 >> 16) & 0xFF;
        uint8_t g2 = (color2 >> 8) & 0xFF;
        uint8_t b2 = color2 & 0xFF;
        
        uint8_t r = (uint8_t)(r1 + (r2 - r1) * t);
        uint8_t g = (uint8_t)(g1 + (g2 - g1) * t);
        uint8_t b = (uint8_t)(b1 + (b2 - b1) * t);
        
        return (r << 16) | (g << 8) | b;
    }
    
    void setBrightness(int brightness) {
        _brightness = constrain(brightness, 0, 255);
        _pixel->setBrightness(_brightness);
    }
    
    // ===== Flash Once and Revert =====
    
    void flashAndRevert(uint32_t color, unsigned long duration = 200) {
        // Store current state
        LEDState prevState = _state;
        uint32_t prevColor = _color;
        
        // Flash
        setSolid(color);
        delay(duration);
        
        // Revert
        _state = prevState;
        _color = prevColor;
        update();
    }
};

#endif