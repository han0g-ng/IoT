/**
 * @file config.h
 * @brief Configuration file for ESP32 Power Monitor System
 * 
 * Contains all configuration parameters including:
 * - WiFi credentials
 * - MQTT broker settings
 * - GPIO pin assignments
 * - Sensor addresses
 * - System parameters
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// DEVICE IDENTIFICATION
// ============================================================================
#define DEVICE_ID           "anh_hong_dep_trai_ittn"
#define DEVICE_NAME         "ESP32 Power Monitor"
#define FIRMWARE_VERSION    "1.0.0"

// ============================================================================
// WIFI CONFIGURATION
// ============================================================================
#define WIFI_SSID           "B2306"        // Thay đổi thành tên WiFi của bạn
#define WIFI_PASSWORD       "Abc@2306"    // Thay đổi thành mật khẩu WiFi
#define WIFI_CONNECT_TIMEOUT 20000                  // Timeout kết nối WiFi (ms)
#define WIFI_RECONNECT_INTERVAL 5000                // Khoảng thời gian thử kết nối lại (ms)

// ============================================================================
// MQTT CONFIGURATION
// ============================================================================
#define MQTT_BROKER         "broker.hivemq.com"     // Địa chỉ MQTT Broker (có thể dùng public broker để test)
#define MQTT_PORT           1883                     // Port MQTT (1883 cho non-SSL, 8883 cho SSL)
#define MQTT_USERNAME       ""                       // Username MQTT (để trống nếu không cần)
#define MQTT_PASSWORD       ""                       // Password MQTT (để trống nếu không cần)
#define MQTT_CLIENT_ID      DEVICE_ID
#define MQTT_RECONNECT_INTERVAL 5000                // Khoảng thời gian thử kết nối lại (ms)
#define MQTT_KEEPALIVE      60                       // Keepalive interval (seconds)

// MQTT Topics Base
#define MQTT_BASE_TOPIC     "devices/" DEVICE_ID

// MQTT Topics - Telemetry (Publish)
#define MQTT_TOPIC_TELEMETRY        MQTT_BASE_TOPIC "/telemetry"
#define MQTT_TOPIC_STATUS           MQTT_BASE_TOPIC "/status"
#define MQTT_TOPIC_ERROR            MQTT_BASE_TOPIC "/error"
#define MQTT_TOPIC_HEARTBEAT        MQTT_BASE_TOPIC "/heartbeat"

// MQTT Topics - Channel 1
#define MQTT_TOPIC_CH1_TELEMETRY    MQTT_BASE_TOPIC "/ch1/telemetry"
#define MQTT_TOPIC_CH1_STATUS       MQTT_BASE_TOPIC "/ch1/status"
#define MQTT_TOPIC_CH1_SWITCH_SET   MQTT_BASE_TOPIC "/ch1/switch/set"
#define MQTT_TOPIC_CH1_SIM_SET      MQTT_BASE_TOPIC "/ch1/sim/set"

// MQTT Topics - Channel 2
#define MQTT_TOPIC_CH2_TELEMETRY    MQTT_BASE_TOPIC "/ch2/telemetry"
#define MQTT_TOPIC_CH2_STATUS       MQTT_BASE_TOPIC "/ch2/status"
#define MQTT_TOPIC_CH2_SWITCH_SET   MQTT_BASE_TOPIC "/ch2/switch/set"
#define MQTT_TOPIC_CH2_SIM_SET      MQTT_BASE_TOPIC "/ch2/sim/set"

// MQTT Topics - Control (Subscribe)
#define MQTT_TOPIC_CONTROL          MQTT_BASE_TOPIC "/control"

// Last Will and Testament
#define MQTT_LWT_TOPIC      MQTT_BASE_TOPIC "/status"
#define MQTT_LWT_MESSAGE    "{\"online\":false}"
#define MQTT_LWT_QOS        1
#define MQTT_LWT_RETAIN     true

// ============================================================================
// GPIO PIN CONFIGURATION
// ============================================================================
// I2C Pins
#define I2C_SDA_PIN         21
#define I2C_SCL_PIN         22

// Channel 1 Control Pins
#define MAIN_SWITCH_PIN_1   25      // Main MOSFET control for Channel 1
#define SIMULATOR_PIN_1     23      // Fault simulator MOSFET for Channel 1

// Channel 2 Control Pins
#define MAIN_SWITCH_PIN_2   26      // Main MOSFET control for Channel 2
#define SIMULATOR_PIN_2     19      // Fault simulator MOSFET for Channel 2

// PWM Configuration for Simulators
#define PWM_FREQUENCY       5000    // PWM frequency in Hz
#define PWM_RESOLUTION      8       // PWM resolution in bits (0-255)
#define PWM_CHANNEL_SIM1    0       // LEDC channel for Simulator 1
#define PWM_CHANNEL_SIM2    1       // LEDC channel for Simulator 2

// ============================================================================
// INA226 SENSOR CONFIGURATION
// ============================================================================
#define INA226_ADDR_CH1     0x40    // I2C address for Channel 1 INA226
#define INA226_ADDR_CH2     0x41    // I2C address for Channel 2 INA226

// INA226 Shunt Resistor Value
#define SHUNT_RESISTOR      0.1     // Shunt resistor value in Ohms (R100 = 0.1Ω)

// INA226 Calibration
#define MAX_EXPECTED_CURRENT 3.0    // Maximum expected current in Amps
#define INA226_CURRENT_LSB   0.0001 // Current LSB = Max Current / 32768

// ============================================================================
// SYSTEM PARAMETERS
// ============================================================================
// Telemetry Intervals
#define TELEMETRY_INTERVAL      1000    // Send telemetry every 1 second (ms)
#define STATUS_INTERVAL         5000    // Send status every 5 seconds (ms)
#define HEARTBEAT_INTERVAL      30000   // Send heartbeat every 30 seconds (ms)

// Safety Thresholds
#define OVERCURRENT_THRESHOLD   3.5     // Overcurrent threshold in Amps
#define OVERVOLTAGE_THRESHOLD   14.0    // Overvoltage threshold in Volts
#define UNDERVOLTAGE_THRESHOLD  10.0    // Undervoltage threshold in Volts
#define OVERCURRENT_DURATION    100     // Duration before triggering protection (ms)

// Channel Names (for display purposes)
#define CHANNEL_1_NAME          "Đèn 1"
#define CHANNEL_2_NAME          "Đèn 2"

// ============================================================================
// DEBUG CONFIGURATION
// ============================================================================
#define DEBUG_ENABLED           true    // Enable/disable debug output
#define DEBUG_SERIAL_BAUD       115200  // Serial baud rate for debugging

#if DEBUG_ENABLED
    #define DEBUG_PRINT(x)      Serial.print(x)
    #define DEBUG_PRINTLN(x)    Serial.println(x)
    #define DEBUG_PRINTF(...)   Serial.printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(...)
#endif

#endif // CONFIG_H
