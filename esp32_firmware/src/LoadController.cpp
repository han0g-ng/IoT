/**
 * @file LoadController.cpp
 * @brief Implementation of Load Control Module
 */

#include "LoadController.h"

// Global instance
LoadController loadController;

LoadController::LoadController() {
    // Initialize channel 1 state
    _channel1.mainSwitch = false;
    _channel1.simValue = 100;      // Default: full conduction (normal operation)
    _channel1.simPWM = 255;
    _channel1.fault = false;
    _channel1.faultReason = "";
    _channel1.lastFaultTime = 0;
    
    // Initialize channel 2 state
    _channel2.mainSwitch = false;
    _channel2.simValue = 100;
    _channel2.simPWM = 255;
    _channel2.fault = false;
    _channel2.faultReason = "";
    _channel2.lastFaultTime = 0;
    
    _channel1Enabled = true;
    _channel2Enabled = true;
    _channel1Changed = false;
    _channel2Changed = false;
}

void LoadController::begin() {
    DEBUG_PRINTLN("Initializing Load Controller...");
    
    // Configure main switch pins as outputs
    pinMode(MAIN_SWITCH_PIN_1, OUTPUT);
    pinMode(MAIN_SWITCH_PIN_2, OUTPUT);
    
    // Initialize main switches to OFF
    digitalWrite(MAIN_SWITCH_PIN_1, LOW);
    digitalWrite(MAIN_SWITCH_PIN_2, LOW);
    
    // Configure PWM for simulators using LEDC
    // Channel 1 simulator
    ledcSetup(PWM_CHANNEL_SIM1, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(SIMULATOR_PIN_1, PWM_CHANNEL_SIM1);
    ledcWrite(PWM_CHANNEL_SIM1, 255);  // Full conduction (normal)
    
    // Channel 2 simulator
    ledcSetup(PWM_CHANNEL_SIM2, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(SIMULATOR_PIN_2, PWM_CHANNEL_SIM2);
    ledcWrite(PWM_CHANNEL_SIM2, 255);  // Full conduction (normal)
    
    DEBUG_PRINTLN("Load Controller initialized");
    DEBUG_PRINTF("  CH1: Main=%d, Sim=%d\n", MAIN_SWITCH_PIN_1, SIMULATOR_PIN_1);
    DEBUG_PRINTF("  CH2: Main=%d, Sim=%d\n", MAIN_SWITCH_PIN_2, SIMULATOR_PIN_2);
}

bool LoadController::setSwitch(uint8_t channel, bool state) {
    ChannelState* ch = getChannelState(channel);
    if (ch == nullptr) return false;
    
    // Check if channel is enabled
    if ((channel == 1 && !_channel1Enabled) || (channel == 2 && !_channel2Enabled)) {
        DEBUG_PRINTF("Channel %d is disabled\n", channel);
        return false;
    }
    
    // Check for fault condition
    if (ch->fault && state) {
        DEBUG_PRINTF("Cannot turn ON channel %d - fault present: %s\n", 
                    channel, ch->faultReason.c_str());
        return false;
    }
    
    ch->mainSwitch = state;
    applyMainSwitch(channel);
    
    // Set state changed flag
    if (channel == 1) _channel1Changed = true;
    else _channel2Changed = true;
    
    DEBUG_PRINTF("Channel %d switch set to %s\n", channel, state ? "ON" : "OFF");
    return true;
}

bool LoadController::getSwitchState(uint8_t channel) {
    ChannelState* ch = getChannelState(channel);
    return (ch != nullptr) ? ch->mainSwitch : false;
}

bool LoadController::toggleSwitch(uint8_t channel) {
    bool currentState = getSwitchState(channel);
    setSwitch(channel, !currentState);
    return !currentState;
}

bool LoadController::setSimulator(uint8_t channel, uint8_t value) {
    ChannelState* ch = getChannelState(channel);
    if (ch == nullptr) return false;
    
    // Clamp value to 0-100
    if (value > 100) value = 100;
    
    ch->simValue = value;
    ch->simPWM = percentToPWM(value);
    applySimulator(channel);
    
    // Set state changed flag
    if (channel == 1) _channel1Changed = true;
    else _channel2Changed = true;
    
    DEBUG_PRINTF("Channel %d simulator set to %d%% (PWM=%d)\n", channel, value, ch->simPWM);
    return true;
}

uint8_t LoadController::getSimulatorValue(uint8_t channel) {
    ChannelState* ch = getChannelState(channel);
    return (ch != nullptr) ? ch->simValue : 0;
}

void LoadController::emergencyShutdown(uint8_t channel, const char* reason) {
    ChannelState* ch = getChannelState(channel);
    if (ch == nullptr) return;
    
    DEBUG_PRINTF("EMERGENCY SHUTDOWN - Channel %d: %s\n", channel, reason);
    
    // Immediately turn off the main switch
    ch->mainSwitch = false;
    ch->fault = true;
    ch->faultReason = reason;
    ch->lastFaultTime = millis();
    
    applyMainSwitch(channel);
    
    // Set state changed flag
    if (channel == 1) _channel1Changed = true;
    else _channel2Changed = true;
}

void LoadController::emergencyShutdownAll(const char* reason) {
    emergencyShutdown(1, reason);
    emergencyShutdown(2, reason);
}

void LoadController::clearFault(uint8_t channel) {
    ChannelState* ch = getChannelState(channel);
    if (ch == nullptr) return;
    
    ch->fault = false;
    ch->faultReason = "";
    
    // Set state changed flag
    if (channel == 1) _channel1Changed = true;
    else _channel2Changed = true;
    
    DEBUG_PRINTF("Fault cleared for channel %d\n", channel);
}

bool LoadController::hasFault(uint8_t channel) {
    ChannelState* ch = getChannelState(channel);
    return (ch != nullptr) ? ch->fault : true;
}

String LoadController::getFaultReason(uint8_t channel) {
    ChannelState* ch = getChannelState(channel);
    return (ch != nullptr) ? ch->faultReason : "Invalid channel";
}

ChannelState* LoadController::getChannelState(uint8_t channel) {
    if (channel == 1) return &_channel1;
    if (channel == 2) return &_channel2;
    return nullptr;
}

void LoadController::setChannelEnabled(uint8_t channel, bool enabled) {
    if (channel == 1) {
        _channel1Enabled = enabled;
        if (!enabled) setSwitch(1, false);
    } else if (channel == 2) {
        _channel2Enabled = enabled;
        if (!enabled) setSwitch(2, false);
    }
}

bool LoadController::hasStateChanged(uint8_t channel) {
    if (channel == 1) return _channel1Changed;
    if (channel == 2) return _channel2Changed;
    return false;
}

void LoadController::clearStateChanged(uint8_t channel) {
    if (channel == 1) _channel1Changed = false;
    else if (channel == 2) _channel2Changed = false;
}

uint8_t LoadController::getMainSwitchPin(uint8_t channel) {
    return (channel == 1) ? MAIN_SWITCH_PIN_1 : MAIN_SWITCH_PIN_2;
}

uint8_t LoadController::getSimulatorPin(uint8_t channel) {
    return (channel == 1) ? SIMULATOR_PIN_1 : SIMULATOR_PIN_2;
}

uint8_t LoadController::getPWMChannel(uint8_t channel) {
    return (channel == 1) ? PWM_CHANNEL_SIM1 : PWM_CHANNEL_SIM2;
}

void LoadController::applyMainSwitch(uint8_t channel) {
    uint8_t pin = getMainSwitchPin(channel);
    ChannelState* ch = getChannelState(channel);
    
    if (ch != nullptr) {
        digitalWrite(pin, ch->mainSwitch ? HIGH : LOW);
    }
}

void LoadController::applySimulator(uint8_t channel) {
    uint8_t pwmChannel = getPWMChannel(channel);
    ChannelState* ch = getChannelState(channel);
    
    if (ch != nullptr) {
        ledcWrite(pwmChannel, ch->simPWM);
    }
}

uint8_t LoadController::percentToPWM(uint8_t percent) {
    // Convert percentage (0-100) to PWM value (0-255)
    // 100% = fully ON (255), 0% = fully OFF (0)
    return map(percent, 0, 100, 0, 255);
}
