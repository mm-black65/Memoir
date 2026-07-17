// TestSD.ino - Test SD Card functionality
#include <FileManager.h>

FileManager fileManager;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("=== Test SD Card ===");
    
    if (fileManager.begin()) {
        Serial.println("✅ SD Card ready!");
        fileManager.listRecordings();
        
        // Test create file
        File test = fileManager.createRecordingFile();
        if (test) {
            test.println("Test file");
            test.close();
            Serial.println("✅ Test file created");
        }
    }
}

void loop() {
    delay(1000);
}
