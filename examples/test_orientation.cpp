#include <Orientation.h>

Orientation detector;
Orientation::Orientation lastOrientation = Orientation::UNKNOWN;

void setup() {
    Serial.begin(115200);
    delay(1000);  // Wait for Serial Monitor (this delay is OK — it's in setup, not loop)
    
    Serial.println("===========================================");
    Serial.println("  OrientationDetector Test");
    Serial.println("===========================================");
    Serial.println();
    
    // Initialize the MPU6050
    if (detector.begin()) {
        Serial.println("Sensor found! Orientation test running...");
    } else {
        Serial.println("Sensor NOT found. Connect MPU6050 and restart.");
        Serial.println("Expected behavior until hardware arrives.");
    }
    Serial.println();
}

void loop() {
    // Non-blocking read — called every loop iteration
    Orientation::Orientation current = detector.read();
    
    // Only print when orientation changes (reduces Serial spam)
    if (current != lastOrientation) {
        lastOrientation = current;
        
        Serial.print("Orientation changed → ");
        Serial.println(Orientation::orientationToString(current));
    }
    
}
