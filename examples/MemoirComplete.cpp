#include <FileManager.h>
#include <AudioManager.h>
#include <GestureManager.h>
#include <VirtualButton.h>
#include <Orientation.h>
#include <Config.h>
#include <LEDManager.h>
#include <BatteryManager.h>
#include <PowerManager.h>

// Global objects
FileManager fileManager;
AudioManager audioManager(&fileManager);
LEDManager led;
BatteryManager battery;
PowerManager power;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    Serial.println("📝 MEMOIR - Smart Voice Pen v1.0");
    Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    
    // === Initialize LED First (visual feedback) ===
    led.begin();
    led.startup();  // White fade in
    delay(1000);
    led.ready();    // Green ready
    
    // === Initialize Battery ===
    battery.begin();
    if (battery.isLow()) {
        led.lowBattery();
        Serial.println("⚠️ Low battery! Please charge.");
    }
    
    // === Initialize SD Card ===
    if (!fileManager.begin()) {
        led.error();
        Serial.println("❌ FATAL: SD Card required!");
        while(1) delay(1000);
    }
    
    // === Initialize Audio ===
    if (!audioManager.begin()) {
        led.error();
        Serial.println("❌ FATAL: Audio system failed!");
        while(1) delay(1000);
    }
    
    // === Initialize Power Management ===
    power.begin();
    power.activity();  // Mark activity
    
    // === Initialize Gestures (placeholder) ===
    Serial.println("✅ System ready!");
    led.ready();
    
    // Print battery status
    Serial.println(battery.getStatusString());
    
    Serial.println("\n📋 Gestures:");
    Serial.println("  Right-up + Single tap: Start/Stop recording");
    Serial.println("  Right-up + Double tap: Transcribe");
    Serial.println("  Down + Single tap: Play last recording");
    Serial.println("  Down + Double tap: Delete last recording");
    Serial.println("\n💡 Touch the sensor to begin!");
}

void loop() {
    // === Check Battery Status ===
    battery.update();
    
    if (battery.isLow() && !audioManager.isRecording()) {
        led.lowBattery();
    }
    
    // === Audio Recording ===
    if (audioManager.isRecording()) {
        audioManager.updateRecording();
        led.recording();
    } else if (audioManager.isPlaying()) {
        audioManager.updatePlayback();
        led.playback();
    } else {
        // Not recording or playing - check for idle
        if (!power.isSleeping()) {
            led.ready();
        }
    }
    
    // === Check Gestures (placeholder - integrate with GestureManager) ===
    // gestureManager.update();
    
    // === Power Management ===
    power.update();
    if (power.shouldSleep() && audioManager.isIdle()) {
        led.sleep();
        delay(100);
        power.goToSleep();
    }
    
    delay(10);
}

// ===== Gesture Callbacks (to be connected) =====
void onStartRecording() {
    audioManager.startRecording();
    led.recording();
    power.activity();
}

void onStopRecording() {
    audioManager.stopRecording();
    led.ready();
    power.activity();
}

void onPlayback() {
    String latest = fileManager.getLatestRecording();
    if (latest.length() > 0) {
        audioManager.playRecording(latest);
        led.playback();
        power.activity();
    } else {
        led.flashAndRevert(COLOR_YELLOW, 500);
    }
}

void onDelete() {
    if (fileManager.deleteLatestRecording()) {
        led.deleting();
        power.activity();
    }
}

void onTranscribe() {
    led.transcribing();
    // Add transcription logic here
    power.activity();
}