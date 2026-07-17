// Config.h
#ifndef Config_h
#define Config_h

// ============ ESP32-C3 Pin Mapping ============

// I2S Audio Pins
#define I2S_BCK_PIN         4
#define I2S_WS_PIN          5
#define MIC_DIN_PIN         6
#define AMP_DIN_PIN         10
#define AMP_SD_PIN          3

// I2C Pins
#define MPU6050_SDA_PIN     2
#define MPU6050_SCL_PIN     1
#define MPU6050_ADDR        0x68

// SD Card SPI Pins
#define SD_CS_PIN           11
#define SD_SCK_PIN          12
#define SD_MOSI_PIN         13
#define SD_MISO_PIN         14

// Inputs
#define TOUCH_PIN           0

// Outputs
#define VIBRATION_PIN       20
#define NEOPIXEL_PIN        21

// Battery
#define BATTERY_ADC_PIN     A0

// ============ System Constants ============

#define SAMPLE_RATE         16000
#define BUFFER_SIZE         1024
#define MAX_RECORDING_TIME  1800000
#define RECORDING_DIR       "/recordings"
#define TEMP_DIR            "/temp"

// Gestures
#define LONG_PRESS_MS       2000    // 2 seconds for AI Assistant
// DO NOT define DOUBLE_TAP_WINDOW here - it's defined in VirtualButton.h

// Power Management
#define INACTIVITY_TIMEOUT  5000

// I2S Configuration
#define I2S_PORT            I2S_NUM_0
#define I2S_DMA_BUF_COUNT   8
#define I2S_DMA_BUF_LEN     256

// NeoPixel
#define NEOPIXEL_BRIGHTNESS 50

// Feature Flags
#define ENABLE_DEBUG        true
#define ENABLE_SLEEP        true
#define ENABLE_BATTERY_MON  true
#define ENABLE_LED          true
#define ENABLE_AI_ASSISTANT true

#endif