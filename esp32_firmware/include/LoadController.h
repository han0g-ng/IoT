/**
 * @file LoadController.h
 * @brief Load Control Module for ESP32 Power Monitor
 * 
 * Manages MOSFET control for both channels including:
 * - Main switch control (ON/OFF)
 * - Fault simulator control (PWM)
 * - Safety protection
 */

#ifndef LOAD_CONTROLLER_H
#define LOAD_CONTROLLER_H

#include <Arduino.h>
#include "config.h"

/**
 * @struct ChannelState
 * @brief Structure to hold channel state information
 */
struct ChannelState {
    bool mainSwitch;        // Main switch state (ON/OFF)
    uint8_t simValue;       // Simulator PWM value (0-100%)
    uint8_t simPWM;         // Actual PWM value (0-255)
    bool fault;             // Fault flag
    String faultReason;     // Fault reason
    unsigned long lastFaultTime; // Timestamp of last fault
};

/**
 * @class LoadController
 * @brief Controls load channels via MOSFETs
 */
class LoadController {
public:
    /**
     * @brief Constructor
     */
    LoadController();
    
    /**
     * @brief Initialize the load controller
     */
    void begin();
    
    /**
     * @brief Set main switch state for a channel
     * @param channel Channel number (1 or 2)
     * @param state Switch state (true = ON, false = OFF)
     * @return true if successful
     */
    bool setSwitch(uint8_t channel, bool state);
    
    /**
     * @brief Get main switch state for a channel
     * @param channel Channel number (1 or 2)
     * @return Switch state
     */
    bool getSwitchState(uint8_t channel);
    
    /**
     * @brief Toggle main switch for a channel
     * @param channel Channel number (1 or 2)
     * @return New switch state
     */
    bool toggleSwitch(uint8_t channel);
    
    /**
     * @brief Set simulator value for a channel (0-100%)
     * @param channel Channel number (1 or 2)
     * @param value Percentage value (0-100)
     * @return true if successful
     */
    bool setSimulator(uint8_t channel, uint8_t value);
    
    /**
     * @brief Get simulator value for a channel
     * @param channel Channel number (1 or 2)
     * @return Simulator percentage value (0-100)
     */
    uint8_t getSimulatorValue(uint8_t channel);
    
    /**
     * @brief Emergency shutdown for a channel
     * @param channel Channel number (1 or 2)
     * @param reason Reason for shutdown
     */
    void emergencyShutdown(uint8_t channel, const char* reason);
    
    /**
     * @brief Emergency shutdown for all channels
     * @param reason Reason for shutdown
     */
    void emergencyShutdownAll(const char* reason);
    
    /**
     * @brief Clear fault for a channel
     * @param channel Channel number (1 or 2)
     */
    void clearFault(uint8_t channel);
    
    /**
     * @brief Check if channel has fault
     * @param channel Channel number (1 or 2)
     * @return true if fault present
     */
    bool hasFault(uint8_t channel);
    
    /**
     * @brief Get fault reason for a channel
     * @param channel Channel number (1 or 2)
     * @return Fault reason string
     */
    String getFaultReason(uint8_t channel);
    
    /**
     * @brief Get channel state
     * @param channel Channel number (1 or 2)
     * @return Pointer to channel state (nullptr if invalid channel)
     */
    ChannelState* getChannelState(uint8_t channel);
    
    /**
     * @brief Enable or disable a channel
     * @param channel Channel number (1 or 2)
     * @param enabled Enable state
     */
    void setChannelEnabled(uint8_t channel, bool enabled);
    
    /**
     * @brief Check if channel state changed since last check
     * @param channel Channel number (1 or 2)
     * @return true if state changed
     */
    bool hasStateChanged(uint8_t channel);
    
    /**
     * @brief Clear state changed flag
     * @param channel Channel number (1 or 2)
     */
    void clearStateChanged(uint8_t channel);
    
private:
    ChannelState _channel1;
    ChannelState _channel2;
    bool _channel1Enabled;
    bool _channel2Enabled;
    bool _channel1Changed;
    bool _channel2Changed;
    
    /**
     * @brief Get pin for main switch
     */
    uint8_t getMainSwitchPin(uint8_t channel);
    
    /**
     * @brief Get pin for simulator
     */
    uint8_t getSimulatorPin(uint8_t channel);
    
    /**
     * @brief Get PWM channel for simulator
     */
    uint8_t getPWMChannel(uint8_t channel);
    
    /**
     * @brief Apply main switch state to hardware
     */
    void applyMainSwitch(uint8_t channel);
    
    /**
     * @brief Apply simulator PWM to hardware
     */
    void applySimulator(uint8_t channel);
    
    /**
     * @brief Convert percentage to PWM value
     */
    uint8_t percentToPWM(uint8_t percent);
};

// Global instance
extern LoadController loadController;

#endif // LOAD_CONTROLLER_H
