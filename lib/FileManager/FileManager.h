// FileManager.h - ESP32 compatible
#ifndef FileManager_h
#define FileManager_h

#include <SPI.h>
#include <SD.h>  // This works with ESP32 built-in SD library
#include "Config.h"

class FileManager {
private:
    bool _initialized = false;

public:
    bool begin() {
        // ESP32-C3 SPI pins
        SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
        
        if (!SD.begin(SD_CS_PIN)) {
            Serial.println("❌ SD Card initialization failed!");
            return false;
        }
        
        _initialized = true;
        Serial.println("✅ SD Card initialized");
        
        // Create directories
        if (!SD.exists(RECORDING_DIR)) {
            SD.mkdir(RECORDING_DIR);
        }
        if (!SD.exists(TEMP_DIR)) {
            SD.mkdir(TEMP_DIR);
        }
        
        return true;
    }
    
    bool isReady() { return _initialized; }
    
    File createRecordingFile() {
        char filename[32];
        snprintf(filename, sizeof(filename), 
                RECORDING_DIR "/rec_%lu.wav", millis());
        return SD.open(filename, FILE_WRITE);
    }
    
    String getLatestRecording() {
        File root = SD.open(RECORDING_DIR);
        if (!root) return "";
        
        String latest = "";
        unsigned long latestTime = 0;
        
        File file = root.openNextFile();
        while (file) {
            if (!file.isDirectory()) {
                String name = file.name();
                if (name.endsWith(".wav")) {
                    // Extract timestamp from filename
                    int start = name.indexOf('_') + 1;
                    int end = name.indexOf('.');
                    if (start > 0 && end > start) {
                        unsigned long time = name.substring(start, end).toInt();
                        if (time > latestTime) {
                            latestTime = time;
                            latest = String(RECORDING_DIR) + "/" + name;
                        }
                    }
                }
            }
            file = root.openNextFile();
        }
        root.close();
        return latest;
    }
    
    bool deleteLatestRecording() {
        String latest = getLatestRecording();
        if (latest.length() > 0) {
            return SD.remove(latest.c_str());
        }
        return false;
    }
};

#endif