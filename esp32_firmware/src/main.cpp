/**
 * @file main.cpp
 * @brief ESP32 Power Monitor & Control System - Main Firmware
 * 
 * Main application file that integrates all modules:
 * - WiFi connectivity
 * - MQTT communication
 * - INA226 sensor reading
 * - MOSFET control
 * - Safety protection
 * 
 * @author IoT Project
 * @version 1.0.0
 */

#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <ArduinoJson.h>

#include "config.h"
#include "INA226.h"
#include "MQTTManager.h"
#include "LoadController.h"

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================

// WiFi client
WiFiClient wifiClient;

// INA226 sensors
INA226 ina226_ch1(INA226_ADDR_CH1);
INA226 ina226_ch2(INA226_ADDR_CH2);

// Sensor data storage
struct SensorData {
    float voltage;
    float current;
    float power;
    bool valid;
    unsigned long lastReadTime;
};

SensorData sensorData[2];  // Index 0 = Channel 1, Index 1 = Channel 2

// Timing variables
unsigned long lastTelemetryTime = 0;
unsigned long lastStatusTime = 0;
unsigned long lastHeartbeatTime = 0;
unsigned long startTime = 0;

// Overcurrent detection
unsigned long overcurrentStartTime[2] = {0, 0};
bool overcurrentDetected[2] = {false, false};

// ============================================================================
// FUNCTION PROTOTYPES
// ============================================================================

void setupWiFi();
void setupSensors();
void setupMQTT();
void handleMQTTMessage(const char* topic, const char* payload);
void readSensors();
void checkSafetyLimits();
void publishTelemetry();
void publishStatus();
void publishHeartbeat();
void handleSerialCommands();

// ============================================================================
// SETUP
// ============================================================================

void setup() {
    // Initialize serial for debugging
    Serial.begin(DEBUG_SERIAL_BAUD);
    delay(1000);
    
    DEBUG_PRINTLN("\n\n========================================");
    DEBUG_PRINTLN("  ESP32 Power Monitor & Control System");
    DEBUG_PRINTF("  Firmware Version: %s\n", FIRMWARE_VERSION);
    DEBUG_PRINTF("  Device ID: %s\n", DEVICE_ID);
    DEBUG_PRINTLN("========================================\n");
    
    startTime = millis();
    
    // Initialize I2C
    DEBUG_PRINTLN("Initializing I2C bus...");
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);  // 400kHz I2C
    
    // Initialize load controller (MOSFETs)
    loadController.begin();
    
    // Initialize sensors
    setupSensors();
    
    // Connect to WiFi
    setupWiFi();
    
    // Setup MQTT
    setupMQTT();
    
    DEBUG_PRINTLN("\n========================================");
    DEBUG_PRINTLN("  System Ready!");
    DEBUG_PRINTLN("========================================\n");
}

// ============================================================================
// MAIN LOOP
// ============================================================================

void loop() {
    unsigned long currentTime = millis();
    
    // Handle MQTT
    mqtt.loop();
    
    // Read sensors periodically
    static unsigned long lastSensorRead = 0;
    if (currentTime - lastSensorRead >= 100) {  // Read every 100ms
        lastSensorRead = currentTime;
        readSensors();
        checkSafetyLimits();
    }
    
    // Publish telemetry
    if (currentTime - lastTelemetryTime >= TELEMETRY_INTERVAL) {
        lastTelemetryTime = currentTime;
        publishTelemetry();
    }
    
    // Publish status (if changed or periodically)
    if (currentTime - lastStatusTime >= STATUS_INTERVAL ||
        loadController.hasStateChanged(1) || loadController.hasStateChanged(2)) {
        lastStatusTime = currentTime;
        publishStatus();
        loadController.clearStateChanged(1);
        loadController.clearStateChanged(2);
    }
    
    // Publish heartbeat
    if (currentTime - lastHeartbeatTime >= HEARTBEAT_INTERVAL) {
        lastHeartbeatTime = currentTime;
        publishHeartbeat();
    }
    
    // Handle serial commands for debugging
    handleSerialCommands();
    
    // Small delay to prevent watchdog issues
    yield();
}

// ============================================================================
// WIFI SETUP
// ============================================================================

void setupWiFi() {
    DEBUG_PRINTLN("Connecting to WiFi...");
    DEBUG_PRINTF("SSID: %s\n", WIFI_SSID);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    unsigned long startAttempt = millis();
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        DEBUG_PRINT(".");
        
        if (millis() - startAttempt > WIFI_CONNECT_TIMEOUT) {
            DEBUG_PRINTLN("\nWiFi connection timeout!");
            DEBUG_PRINTLN("System will continue without WiFi");
            DEBUG_PRINTLN("Check your WiFi credentials in config.h");
            return;
        }
    }
    
    DEBUG_PRINTLN("\nWiFi connected!");
    DEBUG_PRINTF("IP Address: %s\n", WiFi.localIP().toString().c_str());
    DEBUG_PRINTF("Signal Strength: %d dBm\n", WiFi.RSSI());
}

// ============================================================================
// SENSOR SETUP
// ============================================================================

void setupSensors() {
    DEBUG_PRINTLN("Initializing INA226 sensors...");
    
    // Initialize Channel 1 INA226
    if (ina226_ch1.begin(&Wire)) {
        DEBUG_PRINTF("INA226 Channel 1 found at 0x%02X\n", INA226_ADDR_CH1);
        ina226_ch1.calibrate(SHUNT_RESISTOR, MAX_EXPECTED_CURRENT);
        ina226_ch1.setAveraging(INA226_AVG_16);
        sensorData[0].valid = true;
    } else {
        DEBUG_PRINTF("INA226 Channel 1 NOT found at 0x%02X!\n", INA226_ADDR_CH1);
        sensorData[0].valid = false;
    }
    
    // Initialize Channel 2 INA226
    if (ina226_ch2.begin(&Wire)) {
        DEBUG_PRINTF("INA226 Channel 2 found at 0x%02X\n", INA226_ADDR_CH2);
        ina226_ch2.calibrate(SHUNT_RESISTOR, MAX_EXPECTED_CURRENT);
        ina226_ch2.setAveraging(INA226_AVG_16);
        sensorData[1].valid = true;
    } else {
        DEBUG_PRINTF("INA226 Channel 2 NOT found at 0x%02X!\n", INA226_ADDR_CH2);
        sensorData[1].valid = false;
    }
}

// ============================================================================
// MQTT SETUP
// ============================================================================

void setupMQTT() {
    DEBUG_PRINTLN("Setting up MQTT...");
    
    mqtt.begin(wifiClient);
    mqtt.setCallback(handleMQTTMessage);
    
    if (WiFi.status() == WL_CONNECTED) {
        mqtt.connect();
    }
}

// ============================================================================
// MQTT MESSAGE HANDLER
// ============================================================================

void handleMQTTMessage(const char* topic, const char* payload) {
    DEBUG_PRINTF("Processing MQTT message: %s = %s\n", topic, payload);
    
    // Parse JSON payload if present
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload);
    
    // Channel 1 Switch Control
    if (strcmp(topic, MQTT_TOPIC_CH1_SWITCH_SET) == 0) {
        if (strcmp(payload, "ON") == 0 || strcmp(payload, "1") == 0 || 
            (doc.containsKey("state") && doc["state"] == true)) {
            loadController.setSwitch(1, true);
        } else if (strcmp(payload, "OFF") == 0 || strcmp(payload, "0") == 0 ||
                   (doc.containsKey("state") && doc["state"] == false)) {
            loadController.setSwitch(1, false);
        } else if (strcmp(payload, "TOGGLE") == 0) {
            loadController.toggleSwitch(1);
        }
    }
    // Channel 2 Switch Control
    else if (strcmp(topic, MQTT_TOPIC_CH2_SWITCH_SET) == 0) {
        if (strcmp(payload, "ON") == 0 || strcmp(payload, "1") == 0 ||
            (doc.containsKey("state") && doc["state"] == true)) {
            loadController.setSwitch(2, true);
        } else if (strcmp(payload, "OFF") == 0 || strcmp(payload, "0") == 0 ||
                   (doc.containsKey("state") && doc["state"] == false)) {
            loadController.setSwitch(2, false);
        } else if (strcmp(payload, "TOGGLE") == 0) {
            loadController.toggleSwitch(2);
        }
    }
    // Channel 1 Simulator Control
    else if (strcmp(topic, MQTT_TOPIC_CH1_SIM_SET) == 0) {
        int value = atoi(payload);
        if (doc.containsKey("value")) {
            value = doc["value"];
        }
        loadController.setSimulator(1, constrain(value, 0, 100));
    }
    // Channel 2 Simulator Control
    else if (strcmp(topic, MQTT_TOPIC_CH2_SIM_SET) == 0) {
        int value = atoi(payload);
        if (doc.containsKey("value")) {
            value = doc["value"];
        }
        loadController.setSimulator(2, constrain(value, 0, 100));
    }
    // General Control
    else if (strcmp(topic, MQTT_TOPIC_CONTROL) == 0) {
        if (!error && doc.containsKey("command")) {
            const char* command = doc["command"];
            
            if (strcmp(command, "reset") == 0) {
                DEBUG_PRINTLN("Reset command received");
                ESP.restart();
            }
            else if (strcmp(command, "clear_fault") == 0) {
                int channel = doc["channel"] | 0;
                if (channel == 1 || channel == 2) {
                    loadController.clearFault(channel);
                } else {
                    loadController.clearFault(1);
                    loadController.clearFault(2);
                }
            }
            else if (strcmp(command, "status") == 0) {
                publishStatus();
            }
        }
    }
}

// ============================================================================
// SENSOR READING
// ============================================================================

void readSensors() {
    // Read Channel 1
    if (sensorData[0].valid) {
        ina226_ch1.readAll(&sensorData[0].voltage, 
                          &sensorData[0].current, 
                          &sensorData[0].power);
        sensorData[0].lastReadTime = millis();
    }
    
    // Read Channel 2
    if (sensorData[1].valid) {
        ina226_ch2.readAll(&sensorData[1].voltage, 
                          &sensorData[1].current, 
                          &sensorData[1].power);
        sensorData[1].lastReadTime = millis();
    }
}

// ============================================================================
// SAFETY LIMITS CHECK
// ============================================================================

void checkSafetyLimits() {
    unsigned long currentTime = millis();
    
    for (int ch = 0; ch < 2; ch++) {
        uint8_t channel = ch + 1;
        
        if (!sensorData[ch].valid) continue;
        if (!loadController.getSwitchState(channel)) continue;  // Only check if switch is ON
        
        // Check overcurrent
        if (sensorData[ch].current > OVERCURRENT_THRESHOLD) {
            if (!overcurrentDetected[ch]) {
                overcurrentDetected[ch] = true;
                overcurrentStartTime[ch] = currentTime;
            } else if (currentTime - overcurrentStartTime[ch] > OVERCURRENT_DURATION) {
                // Overcurrent persisted - trigger emergency shutdown
                char reason[64];
                snprintf(reason, sizeof(reason), "Overcurrent: %.2fA", sensorData[ch].current);
                loadController.emergencyShutdown(channel, reason);
                
                // Publish error
                mqtt.publishError(channel, "OVERCURRENT", reason, sensorData[ch].current);
                
                DEBUG_PRINTF("⚠️ OVERCURRENT on Channel %d: %.2fA\n", channel, sensorData[ch].current);
            }
        } else {
            overcurrentDetected[ch] = false;
        }
        
        // Check overvoltage
        if (sensorData[ch].voltage > OVERVOLTAGE_THRESHOLD) {
            char reason[64];
            snprintf(reason, sizeof(reason), "Overvoltage: %.2fV", sensorData[ch].voltage);
            loadController.emergencyShutdown(channel, reason);
            mqtt.publishError(channel, "OVERVOLTAGE", reason, sensorData[ch].voltage);
            
            DEBUG_PRINTF("⚠️ OVERVOLTAGE on Channel %d: %.2fV\n", channel, sensorData[ch].voltage);
        }
        
        // Check undervoltage (warning only)
        if (sensorData[ch].voltage > 0 && sensorData[ch].voltage < UNDERVOLTAGE_THRESHOLD) {
            static unsigned long lastUnderVoltageWarning[2] = {0, 0};
            if (currentTime - lastUnderVoltageWarning[ch] > 5000) {  // Warn every 5 seconds
                lastUnderVoltageWarning[ch] = currentTime;
                char reason[64];
                snprintf(reason, sizeof(reason), "Undervoltage: %.2fV", sensorData[ch].voltage);
                mqtt.publishError(channel, "UNDERVOLTAGE", reason, sensorData[ch].voltage);
                
                DEBUG_PRINTF("⚠️ UNDERVOLTAGE on Channel %d: %.2fV\n", channel, sensorData[ch].voltage);
            }
        }
    }
}

// ============================================================================
// MQTT PUBLISHING
// ============================================================================

void publishTelemetry() {
    if (!mqtt.isConnected()) return;
    
    // Publish combined telemetry
    mqtt.publishAllTelemetry(
        sensorData[0].voltage, sensorData[0].current, sensorData[0].power,
        sensorData[1].voltage, sensorData[1].current, sensorData[1].power
    );
    
    // Also publish individual channel telemetry
    mqtt.publishTelemetry(1, sensorData[0].voltage, sensorData[0].current, sensorData[0].power);
    mqtt.publishTelemetry(2, sensorData[1].voltage, sensorData[1].current, sensorData[1].power);
}

void publishStatus() {
    if (!mqtt.isConnected()) return;
    
    // Publish channel status
    mqtt.publishChannelStatus(1, loadController.getSwitchState(1), loadController.getSimulatorValue(1));
    mqtt.publishChannelStatus(2, loadController.getSwitchState(2), loadController.getSimulatorValue(2));
}

void publishHeartbeat() {
    if (!mqtt.isConnected()) return;
    
    unsigned long uptime = (millis() - startTime) / 1000;
    mqtt.publishHeartbeat(uptime, ESP.getFreeHeap());
}

// ============================================================================
// SERIAL COMMAND HANDLER (FOR DEBUGGING)
// ============================================================================

void handleSerialCommands() {
    if (!Serial.available()) return;
    
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "status") {
        DEBUG_PRINTLN("\n--- System Status ---");
        DEBUG_PRINTF("WiFi: %s\n", WiFi.isConnected() ? "Connected" : "Disconnected");
        DEBUG_PRINTF("IP: %s\n", WiFi.localIP().toString().c_str());
        DEBUG_PRINTF("MQTT: %s\n", mqtt.isConnected() ? "Connected" : "Disconnected");
        DEBUG_PRINTF("Free Heap: %d bytes\n", ESP.getFreeHeap());
        DEBUG_PRINTF("Uptime: %lu seconds\n", (millis() - startTime) / 1000);
        
        DEBUG_PRINTLN("\n--- Channel 1 ---");
        DEBUG_PRINTF("Sensor: %s\n", sensorData[0].valid ? "OK" : "Not Found");
        DEBUG_PRINTF("Voltage: %.3f V\n", sensorData[0].voltage);
        DEBUG_PRINTF("Current: %.4f A\n", sensorData[0].current);
        DEBUG_PRINTF("Power: %.3f W\n", sensorData[0].power);
        DEBUG_PRINTF("Switch: %s\n", loadController.getSwitchState(1) ? "ON" : "OFF");
        DEBUG_PRINTF("Simulator: %d%%\n", loadController.getSimulatorValue(1));
        DEBUG_PRINTF("Fault: %s\n", loadController.hasFault(1) ? loadController.getFaultReason(1).c_str() : "None");
        
        DEBUG_PRINTLN("\n--- Channel 2 ---");
        DEBUG_PRINTF("Sensor: %s\n", sensorData[1].valid ? "OK" : "Not Found");
        DEBUG_PRINTF("Voltage: %.3f V\n", sensorData[1].voltage);
        DEBUG_PRINTF("Current: %.4f A\n", sensorData[1].current);
        DEBUG_PRINTF("Power: %.3f W\n", sensorData[1].power);
        DEBUG_PRINTF("Switch: %s\n", loadController.getSwitchState(2) ? "ON" : "OFF");
        DEBUG_PRINTF("Simulator: %d%%\n", loadController.getSimulatorValue(2));
        DEBUG_PRINTF("Fault: %s\n", loadController.hasFault(2) ? loadController.getFaultReason(2).c_str() : "None");
    }
    else if (command == "on1") {
        loadController.setSwitch(1, true);
        DEBUG_PRINTLN("Channel 1 ON");
    }
    else if (command == "off1") {
        loadController.setSwitch(1, false);
        DEBUG_PRINTLN("Channel 1 OFF");
    }
    else if (command == "on2") {
        loadController.setSwitch(2, true);
        DEBUG_PRINTLN("Channel 2 ON");
    }
    else if (command == "off2") {
        loadController.setSwitch(2, false);
        DEBUG_PRINTLN("Channel 2 OFF");
    }
    else if (command.startsWith("sim1 ")) {
        int value = command.substring(5).toInt();
        loadController.setSimulator(1, value);
        DEBUG_PRINTF("Channel 1 Simulator: %d%%\n", value);
    }
    else if (command.startsWith("sim2 ")) {
        int value = command.substring(5).toInt();
        loadController.setSimulator(2, value);
        DEBUG_PRINTF("Channel 2 Simulator: %d%%\n", value);
    }
    else if (command == "clear1") {
        loadController.clearFault(1);
        DEBUG_PRINTLN("Channel 1 fault cleared");
    }
    else if (command == "clear2") {
        loadController.clearFault(2);
        DEBUG_PRINTLN("Channel 2 fault cleared");
    }
    else if (command == "scan") {
        DEBUG_PRINTLN("I2C Scanning...");
        for (uint8_t addr = 1; addr < 127; addr++) {
            Wire.beginTransmission(addr);
            if (Wire.endTransmission() == 0) {
                DEBUG_PRINTF("Found device at 0x%02X\n", addr);
            }
        }
        DEBUG_PRINTLN("Scan complete");
    }
    else if (command == "restart") {
        DEBUG_PRINTLN("Restarting...");
        ESP.restart();
    }
    else if (command == "help") {
        DEBUG_PRINTLN("\n--- Available Commands ---");
        DEBUG_PRINTLN("status   - Show system status");
        DEBUG_PRINTLN("on1/off1 - Turn channel 1 ON/OFF");
        DEBUG_PRINTLN("on2/off2 - Turn channel 2 ON/OFF");
        DEBUG_PRINTLN("sim1 XX  - Set channel 1 simulator (0-100)");
        DEBUG_PRINTLN("sim2 XX  - Set channel 2 simulator (0-100)");
        DEBUG_PRINTLN("clear1   - Clear channel 1 fault");
        DEBUG_PRINTLN("clear2   - Clear channel 2 fault");
        DEBUG_PRINTLN("scan     - Scan I2C bus");
        DEBUG_PRINTLN("restart  - Restart ESP32");
        DEBUG_PRINTLN("help     - Show this help");
    }
}
