#ifndef AudioManager_h
#define AudioManager_h

#include <driver/i2s.h>
#include <Config.h>
#include <FileManager.h>

struct WAVHeader {
    char chunkID[4];        // "RIFF"
    uint32_t chunkSize;     // File size - 8
    char format[4];         // "WAVE"
    char subchunk1ID[4];    // "fmt "
    uint32_t subchunk1Size; // 16
    uint16_t audioFormat;   // 1 (PCM)
    uint16_t numChannels;   // 1 (mono)
    uint32_t sampleRate;    // 16000
    uint32_t byteRate;      // sampleRate * numChannels * bitsPerSample/8
    uint16_t blockAlign;    // numChannels * bitsPerSample/8
    uint16_t bitsPerSample; // 16
    char subchunk2ID[4];    // "data"
    uint32_t subchunk2Size; // Data size
};

class AudioManager {
private:
    enum AudioState {
        STATE_IDLE,
        STATE_RECORDING,
        STATE_PLAYING,
        STATE_ERROR
    };
    
    AudioState _state = STATE_IDLE;
    FileManager* _fileManager;
    
    // Recording
    int16_t _buffer[BUFFER_SIZE];
    int _bufferIndex = 0;
    File _recordingFile;
    unsigned long _recordingStartTime;
    unsigned long _lastReminderTime;
    
    // Playback
    File _playbackFile;
    
    // Callbacks
    void (*_onRecordingStart)() = nullptr;
    void (*_onRecordingStop)() = nullptr;
    void (*_onPlaybackStart)() = nullptr;
    void (*_onPlaybackStop)() = nullptr;
    void (*_onError)(const char*) = nullptr;
    
public:
    AudioManager(FileManager* fm) : _fileManager(fm) {}
    
    bool begin() {
        if (!_fileManager->isReady()) {
            return false;
        }
        
        // Initialize I2S for microphone (RX)
        i2s_config_t i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
            .sample_rate = SAMPLE_RATE,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
            .communication_format = I2S_COMM_FORMAT_I2S,
            .intr_alloc_flags = 0,
            .dma_buf_count = I2S_DMA_BUF_COUNT,
            .dma_buf_len = I2S_DMA_BUF_LEN,
            .use_apll = false
        };
        
        i2s_pin_config_t pin_config = {
            .bck_io_num = I2S_BCK_PIN,
            .ws_io_num = I2S_WS_PIN,
            .data_out_num = I2S_PIN_NO_CHANGE,
            .data_in_num = MIC_DIN_PIN
        };
        
        esp_err_t err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
        if (err != ESP_OK) {
            if (_onError) _onError("I2S install failed");
            return false;
        }
        
        i2s_set_pin(I2S_PORT, &pin_config);
        Serial.println("✅ AudioManager initialized");
        return true;
    }
    
    void startRecording() {
        if (_state != STATE_IDLE) {
            if (_onError) _onError("Already recording/playing");
            return;
        }
        
        _state = STATE_RECORDING;
        
        _recordingFile = _fileManager->createRecordingFile();
        if (!_recordingFile) {
            _state = STATE_ERROR;
            if (_onError) _onError("Failed to create file");
            return;
        }
        
        WAVHeader header = createWAVHeader();
        _recordingFile.write((uint8_t*)&header, sizeof(header));
        
        _bufferIndex = 0;
        _recordingStartTime = millis();
        _lastReminderTime = millis();
        
        if (_onRecordingStart) _onRecordingStart();
        Serial.println("🎙️ Recording started");
    }
    
    void stopRecording() {
        if (_state != STATE_RECORDING) {
            if (_onError) _onError("Not recording");
            return;
        }
        
        flushBuffer();
        updateWAVHeader(_recordingFile);
        _recordingFile.close();
        
        _state = STATE_IDLE;
        
        if (_onRecordingStop) _onRecordingStop();
        Serial.println("⏹️ Recording stopped");
    }
    
    void updateRecording() {
        if (_state != STATE_RECORDING) return;
        
        int32_t sample = 0;
        size_t bytesRead = 0;
        
        i2s_read(I2S_PORT, &sample, sizeof(sample), &bytesRead, 10);
        
        if (bytesRead > 0) {
            int16_t audioValue = (int16_t)(sample >> 16);
            _buffer[_bufferIndex++] = audioValue;
            
            if (_bufferIndex >= BUFFER_SIZE) {
                flushBuffer();
            }
        }
        
        if (millis() - _recordingStartTime > MAX_RECORDING_TIME) {
            Serial.println("⏰ Max recording time reached");
            stopRecording();
            return;
        }
        
        if (millis() - _lastReminderTime > 300000) {
            _lastReminderTime = millis();
            Serial.println("🔔 5 minutes recording");
        }
    }
    
    void flushBuffer() {
        if (_bufferIndex == 0) return;
        
        size_t bytesWritten = _recordingFile.write(
            (uint8_t*)_buffer, 
            _bufferIndex * sizeof(int16_t)
        );
        
        if (bytesWritten != _bufferIndex * sizeof(int16_t)) {
            if (_onError) _onError("SD write error");
        }
        
        _bufferIndex = 0;
    }
    
    void playRecording(const String& filename) {
        if (_state != STATE_IDLE) {
            if (_onError) _onError("Already recording/playing");
            return;
        }
        
        _playbackFile = SD.open(filename.c_str(), FILE_READ);
        if (!_playbackFile) {
            if (_onError) _onError("File not found");
            return;
        }
        
        _state = STATE_PLAYING;
        
        _playbackFile.seek(44);
        
        pinMode(AMP_SD_PIN, OUTPUT);
        digitalWrite(AMP_SD_PIN, HIGH);
        
        if (_onPlaybackStart) _onPlaybackStart();
        Serial.printf("🔊 Playing: %s\n", filename.c_str());
    }
    
    void updatePlayback() {
        if (_state != STATE_PLAYING) return;
        
        uint8_t buffer[512];
        int bytesRead = _playbackFile.read(buffer, sizeof(buffer));
        
        if (bytesRead == 0) {
            stopPlayback();
            return;
        }
        
        i2s_set_pin(I2S_PORT, getOutputPinConfig());
        
        for (int i = 0; i < bytesRead; i += 2) {
            if (i + 1 < bytesRead) {
                int16_t sample = (buffer[i+1] << 8) | buffer[i];
                size_t bytesWritten;
                i2s_write(I2S_PORT, &sample, sizeof(sample), &bytesWritten, 100);
            }
        }
        
        i2s_set_pin(I2S_PORT, getInputPinConfig());
    }
    
    void stopPlayback() {
        if (_state != STATE_PLAYING) return;
        
        _playbackFile.close();
        digitalWrite(AMP_SD_PIN, LOW);
        
        _state = STATE_IDLE;
        
        if (_onPlaybackStop) _onPlaybackStop();
        Serial.println("⏹️ Playback stopped");
    }
    
    WAVHeader createWAVHeader() {
        WAVHeader header;
        strcpy(header.chunkID, "RIFF");
        header.chunkSize = 0;
        strcpy(header.format, "WAVE");
        strcpy(header.subchunk1ID, "fmt ");
        header.subchunk1Size = 16;
        header.audioFormat = 1;
        header.numChannels = 1;
        header.sampleRate = SAMPLE_RATE;
        header.bitsPerSample = 16;
        header.byteRate = SAMPLE_RATE * 1 * 2;
        header.blockAlign = 2;
        strcpy(header.subchunk2ID, "data");
        header.subchunk2Size = 0;
        return header;
    }
    
    void updateWAVHeader(File& file) {
        uint32_t fileSize = file.size();
        uint32_t dataSize = fileSize - 44;
        
        file.seek(4);
        file.write((uint8_t*)&(fileSize - 8), 4);
        
        file.seek(40);
        file.write((uint8_t*)&dataSize, 4);
    }
    
    i2s_pin_config_t getInputPinConfig() {
        i2s_pin_config_t config = {
            .bck_io_num = I2S_BCK_PIN,
            .ws_io_num = I2S_WS_PIN,
            .data_out_num = I2S_PIN_NO_CHANGE,
            .data_in_num = MIC_DIN_PIN
        };
        return config;
    }
    
    i2s_pin_config_t getOutputPinConfig() {
        i2s_pin_config_t config = {
            .bck_io_num = I2S_BCK_PIN,
            .ws_io_num = I2S_WS_PIN,
            .data_out_num = AMP_DIN_PIN,
            .data_in_num = I2S_PIN_NO_CHANGE
        };
        return config;
    }
    
    // Callback setters
    void onRecordingStart(void (*cb)()) { _onRecordingStart = cb; }
    void onRecordingStop(void (*cb)()) { _onRecordingStop = cb; }
    void onPlaybackStart(void (*cb)()) { _onPlaybackStart = cb; }
    void onPlaybackStop(void (*cb)()) { _onPlaybackStop = cb; }
    void onError(void (*cb)(const char*)) { _onError = cb; }
    
    // Getters
    bool isRecording() { return _state == STATE_RECORDING; }
    bool isPlaying() { return _state == STATE_PLAYING; }
    bool isIdle() { return _state == STATE_IDLE; }
};

#endif
