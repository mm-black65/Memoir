// TestAudio.ino - Test Audio recording
#include <FileManager.h>
#include <AudioManager.h>

FileManager fileManager;
AudioManager audioManager(&fileManager);

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("=== Test Audio ===");
    
    fileManager.begin();
    audioManager.begin();
    
    audioManager.onRecordingStart([]() {
        Serial.println("🎙️ Recording started");
    });
    
    audioManager.onRecordingStop([]() {
        Serial.println("⏹️ Recording stopped");
    });
    
    Serial.println("Recording for 3 seconds...");
    audioManager.startRecording();
    delay(3000);
    audioManager.stopRecording();
    
    Serial.println("Playing latest recording...");
    String latest = fileManager.getLatestRecording();
    if (latest.length() > 0) {
        audioManager.playRecording(latest);
        delay(3000);
    }
}

void loop() {
    audioManager.updateRecording();
    audioManager.updatePlayback();
}
