// Day15_SD_Setup.ino
#include <SPI.h>
#include <SD.h>

// ESP32-C3 SD Card Pins
#define SD_CS_PIN   11
#define SD_SCK_PIN  12
#define SD_MOSI_PIN 13
#define SD_MISO_PIN 14

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("=== Memoir SD Card Test ===");
    
    // Initialize SPI with custom pins
    SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
    
    // Initialize SD card
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("❌ SD Card initialization failed!");
        Serial.println("   Check wiring:");
        Serial.println("   - CS  → Pin 11");
        Serial.println("   - SCK → Pin 12");
        Serial.println("   - MOSI→ Pin 13");
        Serial.println("   - MISO→ Pin 14");
        return;
    }
    
    Serial.println("✅ SD Card initialized!");
    
    // Create directories
    if (!SD.exists("/recordings")) {
        if (SD.mkdir("/recordings")) {
            Serial.println("✅ Created /recordings directory");
        }
    }
    
    if (!SD.exists("/temp")) {
        if (SD.mkdir("/temp")) {
            Serial.println("✅ Created /temp directory");
        }
    }
    
    // Test file creation
    File testFile = SD.open("/test.txt", FILE_WRITE);
    if (testFile) {
        testFile.println("Memoir Test File");
        testFile.println("Created: " + String(millis()));
        testFile.close();
        Serial.println("✅ Created test file: /test.txt");
    }
    
    // Read test file
    testFile = SD.open("/test.txt", FILE_READ);
    if (testFile) {
        Serial.println("📄 Content of /test.txt:");
        while (testFile.available()) {
            Serial.write(testFile.read());
        }
        testFile.close();
    }
    
    // List all files
    Serial.println("\n📁 Files on SD card:");
    File root = SD.open("/");
    while (File file = root.openNextFile()) {
        if (file.isDirectory()) {
            Serial.print("  📁 ");
            Serial.println(file.name());
        } else {
            Serial.print("  📄 ");
            Serial.print(file.name());
            Serial.print(" (");
            Serial.print(file.size());
            Serial.println(" bytes)");
        }
    }
}

void loop() {}
