/**
 * @file MQTTManager.cpp
 * @brief Implementation of MQTT Communication Manager
 */

#include "MQTTManager.h"

// Static instance pointer for callback
static MQTTManager* _instance = nullptr;

// Global instance
MQTTManager mqtt;

MQTTManager::MQTTManager() {
    _mqttClient = nullptr;
    _userCallback = nullptr;
    _lastError[0] = '\0';
    _lastReconnectAttempt = 0;
    _instance = this;
}

bool MQTTManager::begin(WiFiClient& wifiClient) {
    _mqttClient = new PubSubClient(wifiClient);
    
    _mqttClient->setServer(MQTT_BROKER, MQTT_PORT);
    _mqttClient->setKeepAlive(MQTT_KEEPALIVE);
    _mqttClient->setCallback(mqttCallback);
    _mqttClient->setBufferSize(1024);  // Increase buffer for JSON messages
    
    DEBUG_PRINTLN("MQTT Manager initialized");
    DEBUG_PRINTF("Broker: %s:%d\n", MQTT_BROKER, MQTT_PORT);
    
    return true;
}

bool MQTTManager::connect() {
    if (_mqttClient == nullptr) {
        setError("MQTT client not initialized");
        return false;
    }
    
    if (_mqttClient->connected()) {
        return true;
    }
    
    DEBUG_PRINTLN("Connecting to MQTT broker...");
    
    bool connected = false;
    
    // Try to connect with LWT (Last Will and Testament)
    if (strlen(MQTT_USERNAME) > 0) {
        connected = _mqttClient->connect(
            MQTT_CLIENT_ID,
            MQTT_USERNAME,
            MQTT_PASSWORD,
            MQTT_LWT_TOPIC,
            MQTT_LWT_QOS,
            MQTT_LWT_RETAIN,
            MQTT_LWT_MESSAGE
        );
    } else {
        connected = _mqttClient->connect(
            MQTT_CLIENT_ID,
            nullptr,
            nullptr,
            MQTT_LWT_TOPIC,
            MQTT_LWT_QOS,
            MQTT_LWT_RETAIN,
            MQTT_LWT_MESSAGE
        );
    }
    
    if (connected) {
        DEBUG_PRINTLN("Connected to MQTT broker!");
        
        // Publish online status
        publishDeviceStatus(true);
        
        // Subscribe to control topics
        subscribeToControlTopics();
        
        return true;
    } else {
        int state = _mqttClient->state();
        char errorMsg[64];
        snprintf(errorMsg, sizeof(errorMsg), "MQTT connection failed, rc=%d", state);
        setError(errorMsg);
        DEBUG_PRINTLN(errorMsg);
        return false;
    }
}

bool MQTTManager::isConnected() {
    return _mqttClient != nullptr && _mqttClient->connected();
}

void MQTTManager::loop() {
    if (_mqttClient == nullptr) return;
    
    if (!_mqttClient->connected()) {
        unsigned long now = millis();
        if (now - _lastReconnectAttempt > MQTT_RECONNECT_INTERVAL) {
            _lastReconnectAttempt = now;
            connect();
        }
    } else {
        _mqttClient->loop();
    }
}

void MQTTManager::disconnect() {
    if (_mqttClient != nullptr && _mqttClient->connected()) {
        publishDeviceStatus(false);
        _mqttClient->disconnect();
    }
}

void MQTTManager::setCallback(MQTTMessageCallback callback) {
    _userCallback = callback;
}

void MQTTManager::mqttCallback(char* topic, byte* payload, unsigned int length) {
    if (_instance == nullptr || _instance->_userCallback == nullptr) return;
    
    // Create null-terminated string from payload
    char* message = new char[length + 1];
    memcpy(message, payload, length);
    message[length] = '\0';
    
    DEBUG_PRINTF("MQTT Received [%s]: %s\n", topic, message);
    
    // Call user callback
    _instance->_userCallback(topic, message);
    
    delete[] message;
}

bool MQTTManager::subscribe(const char* topic) {
    if (!isConnected()) return false;
    
    bool success = _mqttClient->subscribe(topic);
    if (success) {
        DEBUG_PRINTF("Subscribed to: %s\n", topic);
    } else {
        DEBUG_PRINTF("Failed to subscribe to: %s\n", topic);
    }
    return success;
}

bool MQTTManager::publish(const char* topic, const char* payload, bool retained) {
    if (!isConnected()) return false;
    
    bool success = _mqttClient->publish(topic, payload, retained);
    if (!success) {
        DEBUG_PRINTF("Failed to publish to: %s\n", topic);
    }
    return success;
}

bool MQTTManager::publishJson(const char* topic, JsonDocument& doc, bool retained) {
    char buffer[512];
    size_t len = serializeJson(doc, buffer, sizeof(buffer));
    return publish(topic, buffer, retained);
}

bool MQTTManager::publishTelemetry(uint8_t channel, float voltage, float current, float power) {
    StaticJsonDocument<256> doc;
    
    doc["channel"] = channel;
    doc["voltage"] = serialized(String(voltage, 3));
    doc["current"] = serialized(String(current, 4));
    doc["power"] = serialized(String(power, 3));
    doc["timestamp"] = millis();
    
    const char* topic = (channel == 1) ? MQTT_TOPIC_CH1_TELEMETRY : MQTT_TOPIC_CH2_TELEMETRY;
    return publishJson(topic, doc);
}

bool MQTTManager::publishAllTelemetry(float voltage1, float current1, float power1,
                                       float voltage2, float current2, float power2) {
    StaticJsonDocument<512> doc;
    
    JsonObject ch1 = doc.createNestedObject("ch1");
    ch1["voltage"] = serialized(String(voltage1, 3));
    ch1["current"] = serialized(String(current1, 4));
    ch1["power"] = serialized(String(power1, 3));
    
    JsonObject ch2 = doc.createNestedObject("ch2");
    ch2["voltage"] = serialized(String(voltage2, 3));
    ch2["current"] = serialized(String(current2, 4));
    ch2["power"] = serialized(String(power2, 3));
    
    doc["timestamp"] = millis();
    doc["device_id"] = DEVICE_ID;
    
    return publishJson(MQTT_TOPIC_TELEMETRY, doc);
}

bool MQTTManager::publishChannelStatus(uint8_t channel, bool switchState, uint8_t simValue) {
    StaticJsonDocument<256> doc;
    
    doc["channel"] = channel;
    doc["switch"] = switchState ? "ON" : "OFF";
    doc["switch_state"] = switchState;
    doc["simulator"] = simValue;
    doc["timestamp"] = millis();
    
    const char* topic = (channel == 1) ? MQTT_TOPIC_CH1_STATUS : MQTT_TOPIC_CH2_STATUS;
    return publishJson(topic, doc, true);  // Retained
}

bool MQTTManager::publishDeviceStatus(bool online) {
    StaticJsonDocument<256> doc;
    
    doc["online"] = online;
    doc["device_id"] = DEVICE_ID;
    doc["device_name"] = DEVICE_NAME;
    doc["firmware"] = FIRMWARE_VERSION;
    doc["timestamp"] = millis();
    
    if (online) {
        doc["ip"] = WiFi.localIP().toString();
        doc["rssi"] = WiFi.RSSI();
    }
    
    return publishJson(MQTT_TOPIC_STATUS, doc, true);  // Retained
}

bool MQTTManager::publishError(uint8_t channel, const char* errorType, const char* message, float value) {
    StaticJsonDocument<384> doc;
    
    doc["device_id"] = DEVICE_ID;
    doc["channel"] = channel;
    doc["error_type"] = errorType;
    doc["message"] = message;
    doc["value"] = value;
    doc["timestamp"] = millis();
    
    // Add severity based on error type
    if (strcmp(errorType, "OVERCURRENT") == 0 || strcmp(errorType, "OVERVOLTAGE") == 0) {
        doc["severity"] = "CRITICAL";
        doc["action"] = "AUTO_SHUTDOWN";
    } else if (strcmp(errorType, "UNDERVOLTAGE") == 0) {
        doc["severity"] = "WARNING";
        doc["action"] = "NOTIFY";
    } else {
        doc["severity"] = "INFO";
        doc["action"] = "NONE";
    }
    
    return publishJson(MQTT_TOPIC_ERROR, doc);
}

bool MQTTManager::publishHeartbeat(unsigned long uptime, uint32_t freeHeap) {
    StaticJsonDocument<256> doc;
    
    doc["device_id"] = DEVICE_ID;
    doc["uptime"] = uptime;
    doc["free_heap"] = freeHeap;
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["timestamp"] = millis();
    
    return publishJson(MQTT_TOPIC_HEARTBEAT, doc);
}

bool MQTTManager::subscribeToControlTopics() {
    bool success = true;
    
    // Subscribe to channel 1 control topics
    success &= subscribe(MQTT_TOPIC_CH1_SWITCH_SET);
    success &= subscribe(MQTT_TOPIC_CH1_SIM_SET);
    
    // Subscribe to channel 2 control topics
    success &= subscribe(MQTT_TOPIC_CH2_SWITCH_SET);
    success &= subscribe(MQTT_TOPIC_CH2_SIM_SET);
    
    // Subscribe to general control topic
    success &= subscribe(MQTT_TOPIC_CONTROL);
    
    return success;
}

const char* MQTTManager::getLastError() {
    return _lastError;
}

void MQTTManager::setError(const char* error) {
    strncpy(_lastError, error, sizeof(_lastError) - 1);
    _lastError[sizeof(_lastError) - 1] = '\0';
}
