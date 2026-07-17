#include <VirtualButton.h>

#define TOUCH_PIN 2

VirtualButton penButton(TOUCH_PIN);

void setup() {
    Serial.begin(115200);
    Serial.println("=== VirtualButton Test ===");
    Serial.println("This code compiles but needs real hardware to run.");
    Serial.println("When you have hardware:");
    Serial.println("  Single tap  → Start/Stop Recording");
    Serial.println("  Double tap  → Transcribe");
    Serial.println("  Long press  → Toggle Sleep");
    Serial.println("===========================");
}

void loop() {
    // Call update() every loop iteration
    VirtualButton::Event event = penButton.update();
    
    // Act on the detected event
    switch (event) {
        case VirtualButton::SINGLE_TAP:
            Serial.println("→ SINGLE TAP: Start/Stop Recording");
            break;
            
        case VirtualButton::DOUBLE_TAP:
            Serial.println("→ DOUBLE TAP: Transcribe Speech");
            break;
            
        case VirtualButton::LONG_PRESS:
            Serial.println("→ LONG PRESS: Toggle Sleep Mode");
            break;
            
        case VirtualButton::NONE:
        default:
            // Nothing happened this loop — that's fine
            break;
    }
}
