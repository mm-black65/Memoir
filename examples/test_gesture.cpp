#include <GestureManager.h>

VirtualButton button(TOUCH_PIN);
Orientation orientation(MPU6050_ADDR);
GestureManager gesture(&button, &orientation);

void setup() {
    Serial.begin(115200);
    
    gesture.onStartRecording([](){ 
        Serial.println("Recording started");
        // Start I2S recording
    });
    
    gesture.onStopRecording([](){ 
        Serial.println("Recording stopped");
        // Save audio file
    });
    
    gesture.onTranscribe([](){ 
        Serial.println("Transcribing...");
        // Process audio with speech-to-text
        gesture.transcriptionFinished(); // When done
    });
    
    gesture.onPlayback([](){ 
        Serial.println("Playing recording");
        // Play audio from SD card
    });
    
    gesture.onDelete([](){ 
        Serial.println("Deleting recording");
        // Delete last file from SD
    });
    
    gesture.onHaptic([](int duration){ 
        // Control vibration motor
        digitalWrite(VIBRATION_PIN, HIGH);
        delay(duration);
        digitalWrite(VIBRATION_PIN, LOW);
    });
}

void loop() {
    gesture.update();
}
