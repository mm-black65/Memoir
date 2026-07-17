/// main.cpp - Minimal working version
#include <Arduino.h>
#include <Wire.h>
#include <driver/i2s.h>

#include "Config.h"
#include "VirtualButton.h"
#include "OrientationDetector.h"
#include "GestureManager.h"

// Global objects
VirtualButton button(TOUCH_PIN);
OrientationDetector orientation(MPU6050_ADDR);
GestureManager gesture(&button, &orientation);

// Callbacks
void handleGesture(GestureAction action) {
    switch (action) {
        case ACTION_START_RECORDING:
            Serial.println("🎙️ Recording started");
            break;
        case ACTION_STOP_RECORDING:
            Serial.println("⏹️ Recording stopped");
            break;
        case ACTION_TRANSCRIBE:
            Serial.println("📝 Transcribing...");
            break;
        case ACTION_PLAYBACK:
            Serial.println("🔊 Playing...");
            break;
        case ACTION_DELETE:
            Serial.println("🗑️ Deleted");
            break;
        case ACTION_AI_ASSISTANT:
            Serial.println("🤖 AI Assistant triggered!");
            break;
        default:
            break;
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("══════════════════════════════════════════════");
    Serial.println("📝 MEMOIR - Smart Voice Pen v2.0");
    Serial.println("   (with AI Assistant)");
    Serial.println("══════════════════════════════════════════════");

    Wire.begin(MPU6050_SDA_PIN, MPU6050_SCL_PIN);
    
    if (!orientation.begin()) {
        Serial.println("❌ MPU6050 not found!");
    } else {
        Serial.println("✅ MPU6050 initialized");
    }

    button.begin();
    gesture.setActionCallback(handleGesture);

    Serial.println("\n✅ System ready!");
    Serial.println("📋 Gestures:");
    Serial.println("  Right-up + Single tap: Start/Stop recording");
    Serial.println("  Right-up + Double tap: Transcribe");
    Serial.println("  Down + Single tap: Play last recording");
    Serial.println("  Down + Double tap: Delete last recording");
    Serial.println("  Long press (2s): 🤖 AI Assistant");
    Serial.println("\n💡 Touch the sensor to begin!");
}

void loop() {
    gesture.update();
    delay(10);
}