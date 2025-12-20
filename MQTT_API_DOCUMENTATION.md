# MQTT API DOCUMENTATION
## ESP32 Power Monitor & Control System

---

## 🔌 MQTT Broker Information

- **Broker**: `broker.hivemq.com`
- **Port**: `1883`
- **Protocol**: MQTT v3.1.1
- **Authentication**: None (public broker)
- **Device ID**: `anh_hong_dep_trai_ittn`

---

## 📡 MQTT Topics Structure

### Base Topic
```
devices/{device_id}/
```

### Topic Hierarchy
```
devices/anh_hong_dep_trai_ittn/
├── telemetry              # Combined telemetry data (publish every 1s)
├── status                 # Device online status (publish every 5s)
├── heartbeat              # System health info (publish every 60s)
├── ch1/                   # Channel 1 - Light 1
│   ├── telemetry         # Channel 1 sensor data (publish every 1s)
│   ├── status            # Channel 1 state (publish every 5s)
│   ├── switch/set        # Control ON/OFF (subscribe)
│   └── sim/set           # Simulator control (subscribe)
└── ch2/                   # Channel 2 - Light 2
    ├── telemetry         # Channel 2 sensor data (publish every 1s)
    ├── status            # Channel 2 state (publish every 5s)
    ├── switch/set        # Control ON/OFF (subscribe)
    └── sim/set           # Simulator control (subscribe)
```

---

## 📤 PUBLISH Topics (ESP32 → Server)

### 1. Combined Telemetry
**Topic**: `devices/anh_hong_dep_trai_ittn/telemetry`  
**Frequency**: Every 1 second  
**Purpose**: Real-time voltage, current, power data for both channels

**JSON Format**:
```json
{
  "ch1": {
    "voltage": 12.219,
    "current": 0.0003,
    "power": 0.004
  },
  "ch2": {
    "voltage": 12.206,
    "current": 0.0001,
    "power": 0.001
  },
  "timestamp": 1126733,
  "device_id": "anh_hong_dep_trai_ittn"
}
```

**Fields**:
- `voltage`: Volts (float, 2 decimals)
- `current`: Amperes (float, 4 decimals)
- `power`: Watts (float, 3 decimals)
- `timestamp`: Milliseconds since boot
- `device_id`: Device identifier

---

### 2. Channel Telemetry
**Topic**: `devices/anh_hong_dep_trai_ittn/ch1/telemetry`  
**Topic**: `devices/anh_hong_dep_trai_ittn/ch2/telemetry`  
**Frequency**: Every 1 second  
**Purpose**: Individual channel sensor readings

**JSON Format**:
```json
{
  "channel": 1,
  "voltage": 12.219,
  "current": 0.0003,
  "power": 0.004,
  "timestamp": 1126736
}
```

**Fields**:
- `channel`: Channel number (1 or 2)
- `voltage`: Volts
- `current`: Amperes
- `power`: Watts
- `timestamp`: Milliseconds since boot

---

### 3. Channel Status
**Topic**: `devices/anh_hong_dep_trai_ittn/ch1/status`  
**Topic**: `devices/anh_hong_dep_trai_ittn/ch2/status`  
**Frequency**: Every 5 seconds  
**Purpose**: Switch state and simulator percentage

**JSON Format**:
```json
{
  "channel": 1,
  "switch": "OFF",
  "switch_state": false,
  "simulator": 100,
  "timestamp": 1123195
}
```

**Fields**:
- `channel`: Channel number
- `switch`: Text state ("ON" or "OFF")
- `switch_state`: Boolean state (true/false)
- `simulator`: PWM percentage (0-100)
  - `100` = Normal operation
  - `70` = 30% power reduction
  - `50` = 50% power reduction
  - `30` = 70% power reduction
  - `0` = Open circuit fault
  - `10` = Overcurrent simulation
- `timestamp`: Milliseconds since boot

---

### 4. Device Status
**Topic**: `devices/anh_hong_dep_trai_ittn/status`  
**Frequency**: Every 5 seconds  
**Purpose**: Device connection and info

**JSON Format**:
```json
{
  "online": true,
  "device_id": "anh_hong_dep_trai_ittn",
  "device_name": "ESP32 Power Monitor",
  "firmware": "1.0.0",
  "timestamp": 4698,
  "ip": "192.168.1.2",
  "rssi": -45
}
```

**Fields**:
- `online`: Connection status (always true when published)
- `device_id`: Device identifier
- `device_name`: Friendly name
- `firmware`: Version string
- `timestamp`: Milliseconds since boot
- `ip`: Local IP address
- `rssi`: WiFi signal strength (dBm)

---

### 5. Heartbeat
**Topic**: `devices/anh_hong_dep_trai_ittn/heartbeat`  
**Frequency**: Every 60 seconds  
**Purpose**: System health monitoring

**JSON Format**:
```json
{
  "device_id": "anh_hong_dep_trai_ittn",
  "uptime": 1262,
  "free_heap": 247364,
  "wifi_rssi": -39,
  "timestamp": 1263763
}
```

**Fields**:
- `device_id`: Device identifier
- `uptime`: Seconds since boot
- `free_heap`: Free RAM in bytes
- `wifi_rssi`: WiFi signal strength (dBm)
- `timestamp`: Milliseconds since boot

---

## 📥 SUBSCRIBE Topics (Server → ESP32)

### 1. Switch Control
**Topic**: `devices/anh_hong_dep_trai_ittn/ch1/switch/set`  
**Topic**: `devices/anh_hong_dep_trai_ittn/ch2/switch/set`  
**Purpose**: Turn device ON/OFF

**Payload Format**: Plain text
```
ON
```
or
```
OFF
```

**Example**:
```bash
# Turn on Channel 1 (Light 1)
mosquitto_pub -h broker.hivemq.com -t "devices/anh_hong_dep_trai_ittn/ch1/switch/set" -m "ON"

# Turn off Channel 2 (Light 2)
mosquitto_pub -h broker.hivemq.com -t "devices/anh_hong_dep_trai_ittn/ch2/switch/set" -m "OFF"
```

**Response**: Updated status published to `ch1/status` or `ch2/status`

---

### 2. Simulator Control
**Topic**: `devices/anh_hong_dep_trai_ittn/ch1/sim/set`  
**Topic**: `devices/anh_hong_dep_trai_ittn/ch2/sim/set`  
**Purpose**: Simulate power faults and variations

**Payload Format**: Plain text (0-100)
```
100
```

**Simulator Values**:
- `100` = Normal operation (100% power)
- `70` = 30% power drop
- `50` = 50% power drop
- `30` = 70% power drop
- `0` = Open circuit fault (no power)
- `10` = Overcurrent simulation

**Example**:
```bash
# Simulate 50% power drop on Channel 1
mosquitto_pub -h broker.hivemq.com -t "devices/anh_hong_dep_trai_ittn/ch1/sim/set" -m "50"

# Reset to normal operation
mosquitto_pub -h broker.hivemq.com -t "devices/anh_hong_dep_trai_ittn/ch1/sim/set" -m "100"

# Simulate open circuit fault
mosquitto_pub -h broker.hivemq.com -t "devices/anh_hong_dep_trai_ittn/ch1/sim/set" -m "0"
```

**Response**: Updated status published to `ch1/status` or `ch2/status`

---

## 💾 Database Schema Recommendations

### Table: `devices`
```sql
CREATE TABLE devices (
    id VARCHAR(50) PRIMARY KEY,
    name VARCHAR(100),
    firmware VARCHAR(20),
    ip VARCHAR(15),
    last_seen TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### Table: `telemetry`
```sql
CREATE TABLE telemetry (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50),
    channel INT,
    voltage DECIMAL(6,3),
    current DECIMAL(8,4),
    power DECIMAL(8,3),
    timestamp BIGINT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_device_channel (device_id, channel),
    INDEX idx_created_at (created_at)
);
```

### Table: `device_status`
```sql
CREATE TABLE device_status (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50),
    channel INT,
    switch_state BOOLEAN,
    simulator_value INT,
    timestamp BIGINT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_device_channel (device_id, channel)
);
```

### Table: `system_health`
```sql
CREATE TABLE system_health (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50),
    uptime INT,
    free_heap INT,
    wifi_rssi INT,
    timestamp BIGINT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_device (device_id)
);
```

---

## 🔄 Integration Flow

### Backend MQTT Subscribe Flow
```
1. Connect to broker.hivemq.com:1883
2. Subscribe to: devices/anh_hong_dep_trai_ittn/#
3. On message received:
   a. Parse JSON payload
   b. Validate data
   c. Save to database
   d. Broadcast to WebSocket clients (for real-time UI)
```

### Control Flow (Web → ESP32)
```
1. User clicks button on web dashboard
2. Frontend sends API request to backend
3. Backend publishes MQTT message:
   - Topic: devices/anh_hong_dep_trai_ittn/ch1/switch/set
   - Payload: "ON" or "OFF"
4. ESP32 receives message and updates hardware
5. ESP32 publishes updated status
6. Backend receives status update
7. Backend broadcasts to WebSocket
8. Frontend updates UI
```

---

## 📊 Real-time Data Processing

### Data Retention Strategy
- **Telemetry**: Keep 7 days of raw data, aggregate hourly for 30 days, daily for 1 year
- **Status**: Keep last 1000 records per channel
- **Heartbeat**: Keep last 100 records

### Aggregation Example (Hourly)
```sql
CREATE TABLE telemetry_hourly (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50),
    channel INT,
    hour TIMESTAMP,
    avg_voltage DECIMAL(6,3),
    avg_current DECIMAL(8,4),
    avg_power DECIMAL(8,3),
    max_power DECIMAL(8,3),
    min_power DECIMAL(8,3),
    INDEX idx_device_hour (device_id, hour)
);
```

---

## 🧪 Testing Tools

### Python Test Scripts (Provided)
1. `mqtt_monitor.py` - Monitor all MQTT messages
2. `test_control.py` - Test switch control
3. `test_simulator.py` - Test fault simulation

### MQTT Explorer
- Download: https://mqtt-explorer.com/
- Connect to: broker.hivemq.com:1883
- Subscribe: devices/anh_hong_dep_trai_ittn/#

### Command Line Testing
```bash
# Subscribe to all topics
mosquitto_sub -h broker.hivemq.com -t "devices/anh_hong_dep_trai_ittn/#" -v

# Turn on light
mosquitto_pub -h broker.hivemq.com -t "devices/anh_hong_dep_trai_ittn/ch1/switch/set" -m "ON"

# Simulate fault
mosquitto_pub -h broker.hivemq.com -t "devices/anh_hong_dep_trai_ittn/ch1/sim/set" -m "50"
```

---

## ⚡ Performance Notes

- **Message Rate**: ~3 messages/second total
- **Payload Size**: 100-300 bytes per message
- **QoS**: 0 (fire and forget)
- **Retained**: No retained messages
- **Network**: WiFi 2.4GHz, RSSI -39 to -45 dBm

---

## 🚨 Error Handling

### Connection Loss
- ESP32 will auto-reconnect to WiFi/MQTT
- Last Will Testament: Not implemented
- Buffering: No message buffering during disconnection

### Data Validation
Backend should validate:
- Voltage: 0-30V range
- Current: 0-10A range
- Power: 0-300W range
- Simulator: 0-100 range
- Timestamp: Monotonic increase (with rollover handling)

---

## 📱 WebSocket Event Format (Recommended)

For real-time web dashboard, broadcast these events:

```javascript
// Telemetry update
{
  "event": "telemetry",
  "data": {
    "ch1": {"voltage": 12.219, "current": 0.0003, "power": 0.004},
    "ch2": {"voltage": 12.206, "current": 0.0001, "power": 0.001},
    "timestamp": 1126733
  }
}

// Status update
{
  "event": "status",
  "channel": 1,
  "data": {
    "switch": "ON",
    "simulator": 100
  }
}

// Device online/offline
{
  "event": "device_status",
  "online": true,
  "ip": "192.168.1.2",
  "rssi": -45
}
```

---

## 📞 Contact

- **Hardware Team**: (Your info here)
- **ESP32 Firmware**: Version 1.0.0
- **Documentation Date**: December 2025
