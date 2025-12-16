/**
 * @file MQTTManager.h
 * @brief MQTT Communication Manager for ESP32 Power Monitor
 * 
 * Handles all MQTT communication including:
 * - Connection management
 * - Topic subscriptions
 * - Message publishing
 * - Message callbacks
 */

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"

// Callback function type for received messages
typedef void (*MQTTMessageCallback)(const char* topic, const char* payload);

/**
 * @class MQTTManager
 * @brief Manages MQTT connections and communications
 */
class MQTTManager {
public:
    /**
     * @brief Constructor
     */
    MQTTManager();
    
    /**
     * @brief Initialize MQTT manager
     * @param wifiClient Reference to WiFiClient
     * @return true if initialization successful
     */
    bool begin(WiFiClient& wifiClient);
    
    /**
     * @brief Connect to MQTT broker
     * @return true if connected
     */
    bool connect();
    
    /**
     * @brief Check if connected to MQTT broker
     * @return true if connected
     */
    bool isConnected();
    
    /**
     * @brief Process MQTT messages (call in loop)
     */
    void loop();
    
    /**
     * @brief Disconnect from MQTT broker
     */
    void disconnect();
    
    /**
     * @brief Set callback for received messages
     * @param callback Function to call when message received
     */
    void setCallback(MQTTMessageCallback callback);
    
    /**
     * @brief Subscribe to a topic
     * @param topic Topic to subscribe
     * @return true if subscription successful
     */
    bool subscribe(const char* topic);
    
    /**
     * @brief Publish message to topic
     * @param topic Topic to publish to
     * @param payload Message payload
     * @param retained Whether to retain message
     * @return true if publish successful
     */
    bool publish(const char* topic, const char* payload, bool retained = false);
    
    /**
     * @brief Publish JSON document to topic
     * @param topic Topic to publish to
     * @param doc JSON document
     * @param retained Whether to retain message
     * @return true if publish successful
     */
    bool publishJson(const char* topic, JsonDocument& doc, bool retained = false);
    
    /**
     * @brief Publish telemetry data for a channel
     * @param channel Channel number (1 or 2)
     * @param voltage Bus voltage (V)
     * @param current Load current (A)
     * @param power Load power (W)
     * @return true if publish successful
     */
    bool publishTelemetry(uint8_t channel, float voltage, float current, float power);
    
    /**
     * @brief Publish combined telemetry for all channels
     * @param voltage1 Channel 1 voltage
     * @param current1 Channel 1 current
     * @param power1 Channel 1 power
     * @param voltage2 Channel 2 voltage
     * @param current2 Channel 2 current
     * @param power2 Channel 2 power
     * @return true if publish successful
     */
    bool publishAllTelemetry(float voltage1, float current1, float power1,
                             float voltage2, float current2, float power2);
    
    /**
     * @brief Publish channel status
     * @param channel Channel number (1 or 2)
     * @param switchState Main switch state
     * @param simValue Simulator PWM value (0-100)
     * @return true if publish successful
     */
    bool publishChannelStatus(uint8_t channel, bool switchState, uint8_t simValue);
    
    /**
     * @brief Publish device status (online/offline)
     * @param online Whether device is online
     * @return true if publish successful
     */
    bool publishDeviceStatus(bool online);
    
    /**
     * @brief Publish error/alert message
     * @param channel Channel number (0 for system)
     * @param errorType Error type string
     * @param message Error message
     * @param value Optional value associated with error
     * @return true if publish successful
     */
    bool publishError(uint8_t channel, const char* errorType, const char* message, float value = 0);
    
    /**
     * @brief Publish heartbeat message
     * @param uptime System uptime in seconds
     * @param freeHeap Free heap memory
     * @return true if publish successful
     */
    bool publishHeartbeat(unsigned long uptime, uint32_t freeHeap);
    
    /**
     * @brief Subscribe to all control topics
     * @return true if all subscriptions successful
     */
    bool subscribeToControlTopics();
    
    /**
     * @brief Get last error message
     * @return Last error message
     */
    const char* getLastError();
    
private:
    PubSubClient* _mqttClient;
    MQTTMessageCallback _userCallback;
    char _lastError[128];
    unsigned long _lastReconnectAttempt;
    
    /**
     * @brief Internal callback for MQTT messages
     */
    static void mqttCallback(char* topic, byte* payload, unsigned int length);
    
    /**
     * @brief Set last error message
     */
    void setError(const char* error);
};

// Global instance
extern MQTTManager mqtt;

#endif // MQTT_MANAGER_H
