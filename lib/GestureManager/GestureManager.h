// GestureManager.h - Gesture detection with AI Assistant
#ifndef GestureManager_h
#define GestureManager_h

#include "VirtualButton.h"
#include "OrientationDetector.h"
#include "Config.h"

enum GestureAction {
    ACTION_NONE,
    ACTION_START_RECORDING,
    ACTION_STOP_RECORDING,
    ACTION_TRANSCRIBE,
    ACTION_PLAYBACK,
    ACTION_DELETE,
    ACTION_AI_ASSISTANT,
    ACTION_SLEEP
};

class GestureManager {
private:
    VirtualButton* _button;
    OrientationDetector* _orientation;
    void (*_actionCallback)(GestureAction) = nullptr;
    unsigned long _longPressStartTime = 0;
    bool _isLongPressDetected = false;
    bool _aiAssistantActive = false;
    unsigned long _lastActivityTime = 0;

public:
    GestureManager(VirtualButton* button, OrientationDetector* orientation)
        : _button(button), _orientation(orientation) {}

    void setActionCallback(void (*cb)(GestureAction)) {
        _actionCallback = cb;
    }

    void update() {
        _button->update();
        _orientation->update();

        if (_button->isPressed()) {
            if (_longPressStartTime == 0) {
                _longPressStartTime = millis();
                _isLongPressDetected = false;
            }

            if (!_isLongPressDetected && (millis() - _longPressStartTime) > LONG_PRESS_MS) {
                _isLongPressDetected = true;
                _aiAssistantActive = true;
                if (_actionCallback) {
                    _actionCallback(ACTION_AI_ASSISTANT);
                }
                _lastActivityTime = millis();
            }
        } else {
            _longPressStartTime = 0;
            _isLongPressDetected = false;
        }

        if (!_isLongPressDetected && !_aiAssistantActive) {
            VirtualButton::Event event = _button->getEvent();
            if (event != VirtualButton::EVENT_NONE) {
                Orientation orientation = _orientation->getStableOrientation();
                processGesture(orientation, event);
                _lastActivityTime = millis();
            }
        }
    }

    void processGesture(Orientation orientation, VirtualButton::Event event) {
        if (event == VirtualButton::EVENT_LONG_PRESS) {
            // Long press already handled
            return;
        }

        if (event == VirtualButton::EVENT_DOUBLE_CLICK) {
            if (orientation == ORIENTATION_RIGHT_UP) {
                if (_actionCallback) _actionCallback(ACTION_TRANSCRIBE);
            } else if (orientation == ORIENTATION_DOWN) {
                if (_actionCallback) _actionCallback(ACTION_DELETE);
            }
        } else if (event == VirtualButton::EVENT_SINGLE_CLICK) {
            if (orientation == ORIENTATION_RIGHT_UP) {
                if (_actionCallback) _actionCallback(ACTION_START_RECORDING);
            } else if (orientation == ORIENTATION_DOWN) {
                if (_actionCallback) _actionCallback(ACTION_PLAYBACK);
            }
        }
    }

    bool isRecording() { return false; }  // Managed externally
    bool isPlaying() { return false; }
    bool isAIAssistantActive() { return _aiAssistantActive; }

    void setAIAssistantActive(bool active) {
        _aiAssistantActive = active;
        if (!active) {
            _longPressStartTime = 0;
            _isLongPressDetected = false;
        }
    }

    void resetLastAction() {}
};

#endif// GestureManager.h - Full State Machine
#ifndef GestureManager_h
#define GestureManager_h

#include "VirtualButton.h"
#include "OrientationDetector.h"
#include "Config.h"

// ===== States =====
enum GestureState {
    STATE_IDLE,
    STATE_WAITING_FOR_TAP,
    STATE_DOUBLE_TAP_WAITING,
    STATE_RECORDING,
    STATE_PLAYING,
    STATE_TRANSCRIBING,
    STATE_DELETING,
    STATE_AI_ASSISTANT,
    STATE_SLEEP_PENDING
};

// ===== Actions =====
enum GestureAction {
    ACTION_NONE,
    ACTION_START_RECORDING,
    ACTION_STOP_RECORDING,
    ACTION_TRANSCRIBE,
    ACTION_PLAYBACK,
    ACTION_DELETE,
    ACTION_AI_ASSISTANT
};

// ===== Callback Type =====
typedef void (*ActionCallback)(GestureAction action);

class GestureManager {
private:
    // ===== Hardware =====
    VirtualButton* _button;
    OrientationDetector* _orientation;
    
    // ===== State Machine =====
    GestureState _state = STATE_IDLE;
    GestureState _previousState = STATE_IDLE;
    GestureAction _lastAction = ACTION_NONE;
    
    // ===== Timing =====
    unsigned long _stateEnterTime = 0;
    unsigned long _lastActivityTime = 0;
    unsigned long _doubleTapStartTime = 0;
    unsigned long _longPressStartTime = 0;
    
    // ===== Flags =====
    bool _isLongPressDetected = false;
    bool _isRecording = false;
    bool _isPlaying = false;
    bool _isTranscribing = false;
    bool _isAIAssistantActive = false;
    
    // ===== Callbacks =====
    ActionCallback _actionCallback = nullptr;
    void (*_hapticCallback)(int) = nullptr;
    
    // ===== Constants =====
    const unsigned long DOUBLE_TAP_WINDOW = 300;
    const unsigned long LONG_PRESS_MS = 2000;
    const unsigned long INACTIVITY_TIMEOUT = 5000;
    const unsigned long DEBOUNCE_DELAY = 50;

public:
    // ===== Constructor =====
    GestureManager(VirtualButton* button, OrientationDetector* orientation)
        : _button(button), _orientation(orientation) {}
    
    // ===== Initialization =====
    void begin() {
        _button->begin();
        _orientation->begin();
        _state = STATE_IDLE;
        _stateEnterTime = millis();
        _lastActivityTime = millis();
        Serial.println("✅ GestureManager initialized (State Machine)");
    }
    
    // ===== Callback Setters =====
    void setActionCallback(ActionCallback cb) { _actionCallback = cb; }
    void onHaptic(void (*cb)(int)) { _hapticCallback = cb; }
    
    // ===== Main Update =====
    void update() {
        _button->update();
        _orientation->update();
        
        // Update activity timer
        if (_button->isPressed()) {
            _lastActivityTime = millis();
        }
        
        // State machine transitions
        switch (_state) {
            case STATE_IDLE:
                handleIdleState();
                break;
            case STATE_WAITING_FOR_TAP:
                handleWaitingState();
                break;
            case STATE_DOUBLE_TAP_WAITING:
                handleDoubleTapWaiting();
                break;
            case STATE_RECORDING:
                handleRecordingState();
                break;
            case STATE_PLAYING:
                handlePlayingState();
                break;
            case STATE_TRANSCRIBING:
                handleTranscribingState();
                break;
            case STATE_DELETING:
                handleDeletingState();
                break;
            case STATE_AI_ASSISTANT:
                handleAIAssistantState();
                break;
            case STATE_SLEEP_PENDING:
                handleSleepPendingState();
                break;
        }
    }
    
    // ===== State Handlers =====
    
    void handleIdleState() {
        if (_button->isPressed()) {
            _state = STATE_WAITING_FOR_TAP;
            _stateEnterTime = millis();
            _longPressStartTime = millis();
            _isLongPressDetected = false;
        }
        
        // Check inactivity timeout
        if (millis() - _lastActivityTime > INACTIVITY_TIMEOUT) {
            transitionTo(STATE_SLEEP_PENDING);
        }
    }
    
    void handleWaitingState() {
        if (!_button->isPressed()) {
            // Button released
            unsigned long pressDuration = millis() - _stateEnterTime;
            
            if (pressDuration > 100 && pressDuration < 1000) {
                // Short press - check for double tap
                if (_doubleTapStartTime > 0 && (millis() - _doubleTapStartTime) < DOUBLE_TAP_WINDOW) {
                    // Double tap detected!
                    _doubleTapStartTime = 0;
                    processGesture(EVENT_DOUBLE_CLICK);
                    transitionTo(STATE_IDLE);
                } else {
                    // First tap - wait for double
                    _doubleTapStartTime = millis();
                    transitionTo(STATE_DOUBLE_TAP_WAITING);
                }
            }
        } else {
            // Still pressed - check for long press
            unsigned long pressDuration = millis() - _stateEnterTime;
            if (!_isLongPressDetected && pressDuration > LONG_PRESS_MS) {
                _isLongPressDetected = true;
                processGesture(EVENT_LONG_PRESS);
                transitionTo(STATE_AI_ASSISTANT);
            }
        }
    }
    
    void handleDoubleTapWaiting() {
        if (_button->isPressed()) {
            // Second tap detected
            unsigned long timeSinceFirstTap = millis() - _doubleTapStartTime;
            if (timeSinceFirstTap < DOUBLE_TAP_WINDOW) {
                _doubleTapStartTime = 0;
                processGesture(EVENT_DOUBLE_CLICK);
                transitionTo(STATE_IDLE);
                return;
            }
        }
        
        // Timeout - single click
        if (millis() - _doubleTapStartTime > DOUBLE_TAP_WINDOW) {
            _doubleTapStartTime = 0;
            processGesture(EVENT_SINGLE_CLICK);
            transitionTo(STATE_IDLE);
        }
    }
    
    void handleRecordingState() {
        // Check for stop gesture
        if (_button->isPressed()) {
            // Wait for release, then treat as stop
            if (!_button->isPressed()) {
                processGesture(EVENT_SINGLE_CLICK);  // Stop recording
                transitionTo(STATE_IDLE);
            }
        }
        
        // Check max recording time (30 minutes)
        if (millis() - _stateEnterTime > MAX_RECORDING_TIME) {
            processGesture(EVENT_SINGLE_CLICK);  // Auto-stop
            transitionTo(STATE_IDLE);
        }
    }
    
    void handlePlayingState() {
        // Check for stop gesture
        if (_button->isPressed() && !_button->isPressed()) {
            processGesture(EVENT_SINGLE_CLICK);  // Stop playback
            transitionTo(STATE_IDLE);
        }
        
        // Check if playback finished (external flag)
        if (_isPlaying == false) {
            transitionTo(STATE_IDLE);
        }
    }
    
    void handleTranscribingState() {
        // Wait for transcription to complete
        if (!_isTranscribing) {
            transitionTo(STATE_IDLE);
        }
    }
    
    void handleDeletingState() {
        // Delete is instantaneous
        transitionTo(STATE_IDLE);
    }
    
    void handleAIAssistantState() {
        // AI Assistant is active
        if (!_isAIAssistantActive) {
            transitionTo(STATE_IDLE);
        }
    }
    
    void handleSleepPendingState() {
        // Enter deep sleep
        if (_actionCallback) {
            _actionCallback(ACTION_NONE);
        }
        // External code will handle sleep
        transitionTo(STATE_IDLE);
    }
    
    // ===== Transition Helper =====
    void transitionTo(GestureState newState) {
        _previousState = _state;
        _state = newState;
        _stateEnterTime = millis();
        Serial.printf("🔄 State: %d → %d\n", _previousState, _state);
    }
    
    // ===== Gesture Processing =====
    void processGesture(ButtonEvent event) {
        Orientation orientation = _orientation->getStableOrientation();
        if (orientation == ORIENTATION_UNKNOWN) {
            orientation = _orientation->getLastStable();
        }
        
        GestureAction action = ACTION_NONE;
        
        if (event == EVENT_LONG_PRESS) {
            action = ACTION_AI_ASSISTANT;
            _isAIAssistantActive = true;
            triggerHaptic(100);
        } else if (event == EVENT_DOUBLE_CLICK) {
            if (orientation == ORIENTATION_RIGHT_UP) {
                action = ACTION_TRANSCRIBE;
                _isTranscribing = true;
                triggerHaptic(50);
            } else if (orientation == ORIENTATION_DOWN) {
                action = ACTION_DELETE;
                triggerHaptic(80);
            }
        } else if (event == EVENT_SINGLE_CLICK) {
            if (orientation == ORIENTATION_RIGHT_UP) {
                if (_isRecording) {
                    action = ACTION_STOP_RECORDING;
                    _isRecording = false;
                } else {
                    action = ACTION_START_RECORDING;
                    _isRecording = true;
                    transitionTo(STATE_RECORDING);
                }
                triggerHaptic(50);
            } else if (orientation == ORIENTATION_DOWN) {
                if (_isPlaying) {
                    action = ACTION_NONE;
                    _isPlaying = false;
                } else {
                    action = ACTION_PLAYBACK;
                    _isPlaying = true;
                    transitionTo(STATE_PLAYING);
                }
                triggerHaptic(50);
            }
        }
        
        if (action != ACTION_NONE && _actionCallback) {
            _actionCallback(action);
        }
        _lastAction = action;
    }
    
    // ===== Trigger Haptic =====
    void triggerHaptic(int duration) {
        if (_hapticCallback) {
            _hapticCallback(duration);
        }
    }
    
    // ===== Getters =====
    GestureState getState() { return _state; }
    bool isRecording() { return _isRecording; }
    bool isPlaying() { return _isPlaying; }
    bool isTranscribing() { return _isTranscribing; }
    bool isAIAssistantActive() { return _isAIAssistantActive; }
    unsigned long getLastActivityTime() { return _lastActivityTime; }
    
    // ===== Setters =====
    void setRecording(bool state) { _isRecording = state; if(!state && _state == STATE_RECORDING) transitionTo(STATE_IDLE); }
    void setPlaying(bool state) { _isPlaying = state; if(!state && _state == STATE_PLAYING) transitionTo(STATE_IDLE); }
    void setTranscribing(bool state) { _isTranscribing = state; if(!state && _state == STATE_TRANSCRIBING) transitionTo(STATE_IDLE); }
    void setAIAssistantActive(bool state) { 
        _isAIAssistantActive = state; 
        if(!state && _state == STATE_AI_ASSISTANT) transitionTo(STATE_IDLE);
    }
};

#endif