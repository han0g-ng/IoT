# ESP32 Power Monitor & Control System

## Hệ thống Giám sát và Điều khiển Công suất IoT

### 📋 Mô tả Dự án

Hệ thống sử dụng ESP32 để giám sát và điều khiển 2 kênh tải DC độc lập thông qua:
- **Cảm biến INA226**: Đo điện áp, dòng điện, công suất
- **MOSFET IRLZ44N**: Điều khiển đóng/ngắt tải và giả lập lỗi
- **MQTT**: Giao tiếp với server/dashboard

---

## 🔧 Yêu cầu Phần cứng

### Linh kiện chính:
- ESP32 Dev Board (38-pin hoặc 30-pin)
- 2x Module INA226 (cảm biến dòng/công suất)
- 4x MOSFET IRLZ44N
- Module Buck LM2596 (12V → 5V)
- Nguồn DC 12V/5A+
- Điện trở: 4x 330Ω, 4x 10kΩ
- Tụ điện: 470µF/25V, 100µF/25V, 100nF

### Sơ đồ chân GPIO:

| Chân ESP32 | GPIO | Chức năng |
|------------|------|-----------|
| 3V3 | - | Nguồn cho INA226 |
| VIN | - | Nguồn 5V từ LM2596 |
| GND | - | Đất chung |
| IO21 | SDA | I2C Data |
| IO22 | SCL | I2C Clock |
| IO25 | MAIN_SWITCH_1 | Công tắc chính Kênh 1 |
| IO23 | SIMULATOR_1 | Giả lập lỗi Kênh 1 |
| IO26 | MAIN_SWITCH_2 | Công tắc chính Kênh 2 |
| IO19 | SIMULATOR_2 | Giả lập lỗi Kênh 2 |

### Địa chỉ I2C INA226:
- **Kênh 1**: `0x40` (mặc định)
- **Kênh 2**: `0x41` (hàn jumper A0 với VCC)

---

## 💻 Cài đặt Môi trường

### 1. Cài đặt PlatformIO

**Cách 1: Extension trong VS Code**
1. Mở VS Code
2. Vào Extensions (Ctrl+Shift+X)
3. Tìm "PlatformIO IDE"
4. Cài đặt và khởi động lại VS Code

**Cách 2: Cài đặt qua pip (Python)**
```bash
pip install platformio
```

### 2. Cài đặt Driver USB

- **CP2102**: [Download từ Silicon Labs](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)
- **CH340**: [Download từ WCH](http://www.wch.cn/downloads/CH341SER_EXE.html)

---

## 🚀 Hướng dẫn Nạp Code

### Bước 1: Cấu hình WiFi và MQTT

Mở file `include/config.h` và thay đổi:

```cpp
// WiFi Configuration
#define WIFI_SSID           "TEN_WIFI_CUA_BAN"
#define WIFI_PASSWORD       "MAT_KHAU_WIFI"

// MQTT Configuration  
#define MQTT_BROKER         "broker.hivemq.com"  // Hoặc địa chỉ broker của bạn
#define MQTT_PORT           1883
#define MQTT_USERNAME       ""                    // Nếu cần xác thực
#define MQTT_PASSWORD       ""
```

### Bước 2: Build Project

**Dùng VS Code + PlatformIO:**
1. Mở thư mục `esp32_firmware` trong VS Code
2. Click vào icon PlatformIO (hình con kiến) ở sidebar
3. Chọn `Build` hoặc nhấn Ctrl+Alt+B

**Dùng Command Line:**
```bash
cd esp32_firmware
pio run
```

### Bước 3: Nạp Code vào ESP32

1. Kết nối ESP32 với máy tính qua USB
2. Kiểm tra COM port (Device Manager trên Windows)
3. Nạp code:

**VS Code + PlatformIO:**
- Click `Upload` hoặc nhấn Ctrl+Alt+U

**Command Line:**
```bash
pio run --target upload
```

### Bước 4: Mở Serial Monitor

```bash
pio device monitor
```

Hoặc trong VS Code: Click icon Serial Monitor ở thanh PlatformIO

---

## 📡 MQTT Topics

### Topics Publish (ESP32 → Server):

| Topic | Mô tả | Payload |
|-------|-------|---------|
| `devices/power_monitor_01/telemetry` | Dữ liệu tổng hợp | JSON với V, I, P cả 2 kênh |
| `devices/power_monitor_01/ch1/telemetry` | Telemetry Kênh 1 | `{"voltage", "current", "power"}` |
| `devices/power_monitor_01/ch2/telemetry` | Telemetry Kênh 2 | `{"voltage", "current", "power"}` |
| `devices/power_monitor_01/status` | Trạng thái thiết bị | `{"online", "ip", "rssi"}` |
| `devices/power_monitor_01/ch1/status` | Trạng thái Kênh 1 | `{"switch", "simulator"}` |
| `devices/power_monitor_01/ch2/status` | Trạng thái Kênh 2 | `{"switch", "simulator"}` |
| `devices/power_monitor_01/error` | Cảnh báo lỗi | `{"error_type", "message", "value"}` |
| `devices/power_monitor_01/heartbeat` | Heartbeat | `{"uptime", "free_heap"}` |

### Topics Subscribe (Server → ESP32):

| Topic | Mô tả | Payload |
|-------|-------|---------|
| `devices/power_monitor_01/ch1/switch/set` | Điều khiển Kênh 1 | `ON`, `OFF`, `TOGGLE` |
| `devices/power_monitor_01/ch2/switch/set` | Điều khiển Kênh 2 | `ON`, `OFF`, `TOGGLE` |
| `devices/power_monitor_01/ch1/sim/set` | Simulator Kênh 1 | `0-100` (%) |
| `devices/power_monitor_01/ch2/sim/set` | Simulator Kênh 2 | `0-100` (%) |
| `devices/power_monitor_01/control` | Lệnh điều khiển | JSON commands |

### Ví dụ Payload:

**Telemetry:**
```json
{
  "ch1": {"voltage": "12.05", "current": "1.234", "power": "14.87"},
  "ch2": {"voltage": "11.98", "current": "0.567", "power": "6.79"},
  "timestamp": 123456789,
  "device_id": "power_monitor_01"
}
```

**Error:**
```json
{
  "device_id": "power_monitor_01",
  "channel": 1,
  "error_type": "OVERCURRENT",
  "message": "Overcurrent: 3.65A",
  "value": 3.65,
  "severity": "CRITICAL",
  "action": "AUTO_SHUTDOWN"
}
```

---

## 🔍 Debug qua Serial

Kết nối Serial Monitor và gõ các lệnh:

| Lệnh | Mô tả |
|------|-------|
| `status` | Hiển thị trạng thái hệ thống |
| `on1` / `off1` | Bật/Tắt Kênh 1 |
| `on2` / `off2` | Bật/Tắt Kênh 2 |
| `sim1 XX` | Đặt Simulator Kênh 1 (0-100%) |
| `sim2 XX` | Đặt Simulator Kênh 2 (0-100%) |
| `clear1` / `clear2` | Xóa lỗi kênh |
| `scan` | Quét bus I2C |
| `restart` | Khởi động lại ESP32 |
| `help` | Hiển thị trợ giúp |

---

## ⚠️ Bảo vệ An toàn

Firmware có các chức năng bảo vệ tự động:

1. **Quá dòng (Overcurrent)**: Ngắt tải khi I > 3.5A trong 100ms
2. **Quá áp (Overvoltage)**: Ngắt tải khi V > 14V
3. **Thấp áp (Undervoltage)**: Cảnh báo khi V < 10V
4. **Last Will Testament**: MQTT broker tự động đánh dấu offline khi mất kết nối

---

## 📁 Cấu trúc Project

```
esp32_firmware/
├── include/
│   ├── config.h           # Cấu hình hệ thống
│   ├── INA226.h           # Thư viện INA226
│   ├── MQTTManager.h      # Quản lý MQTT
│   └── LoadController.h   # Điều khiển MOSFET
├── src/
│   ├── main.cpp           # Firmware chính
│   ├── INA226.cpp         # Implementation INA226
│   ├── MQTTManager.cpp    # Implementation MQTT
│   └── LoadController.cpp # Implementation Load Control
├── platformio.ini         # Cấu hình PlatformIO
└── README.md              # File này
```

---

## 🔄 Troubleshooting

### ESP32 không kết nối WiFi:
- Kiểm tra SSID và password trong `config.h`
- Đảm bảo WiFi là 2.4GHz (ESP32 không hỗ trợ 5GHz)

### Không đọc được INA226:
- Chạy lệnh `scan` để kiểm tra địa chỉ I2C
- Kiểm tra kết nối SDA (IO21), SCL (IO22)
- Kiểm tra jumper A0 trên INA226 kênh 2

### MQTT không kết nối:
- Kiểm tra địa chỉ broker
- Đảm bảo firewall không chặn port 1883
- Thử dùng public broker: `broker.hivemq.com`

### Upload thất bại:
- Cài đặt đúng driver USB (CP2102/CH340)
- Giữ nút BOOT trên ESP32 khi upload
- Thử reset ESP32 và upload lại

---

## 🌐 Hướng dẫn Phát triển Web Dashboard

> **Mục tiêu**: Xây dựng web dashboard để hiển thị realtime và điều khiển ESP32 từ xa
> **Dành cho**: Dự án học tập, demo, prototype

---

## ⚡ Quick Start (10 phút)

### Cách nhanh nhất: Frontend kết nối trực tiếp MQTT

**Bước 1: ESP32 đã publish data lên HiveMQ**
- Đảm bảo ESP32 đã nạp firmware và kết nối MQTT broker
- Check Serial Monitor thấy "Connected to MQTT broker"

**Bước 2: Tạo file HTML đơn giản**
```html
<!DOCTYPE html>
<html>
<head>
    <title>Power Monitor Dashboard</title>
    <script src="https://unpkg.com/mqtt@4.3.7/dist/mqtt.min.js"></script>
</head>
<body>
    <h1>ESP32 Power Monitor</h1>
    <div id="data">Waiting for data...</div>
    
    <button onclick="controlSwitch(1, 'ON')">CH1 ON</button>
    <button onclick="controlSwitch(1, 'OFF')">CH1 OFF</button>
    <button onclick="controlSwitch(2, 'ON')">CH2 ON</button>
    <button onclick="controlSwitch(2, 'OFF')">CH2 OFF</button>
    
    <script>
        const client = mqtt.connect('wss://broker.hivemq.com:8884/mqtt');
        
        client.on('connect', () => {
            console.log('Connected!');
            client.subscribe('devices/power_monitor_01/telemetry');
            client.subscribe('devices/power_monitor_01/+/status');
        });
        
        client.on('message', (topic, message) => {
            const data = JSON.parse(message.toString());
            document.getElementById('data').innerHTML = 
                '<pre>' + JSON.stringify(data, null, 2) + '</pre>';
        });
        
        function controlSwitch(channel, action) {
            const topic = `devices/power_monitor_01/ch${channel}/switch/set`;
            client.publish(topic, action);
            console.log(`Published ${action} to ${topic}`);
        }
    </script>
</body>
</html>
```

**Bước 3: Mở file HTML trong browser**
- Double-click file HTML
- Mở Console (F12) để xem logs
- Done! 🎉

---

### 🏗️ Kiến trúc Hệ thống

Có 2 cách triển khai:

#### **Option A: Frontend kết nối trực tiếp MQTT (KHUYẾN NGHỊ cho học tập)**
```
ESP32 → HiveMQ Cloud ← Frontend (MQTT.js over WebSocket)
```
- **Ưu điểm**: CỰC KỲ đơn giản, không cần backend, không cần database, realtime tốt
- **Nhược điểm**: Không lưu lịch sử (chấp nhận được cho demo)
- **Thời gian setup**: 10 phút

#### **Option B: Backend Bridge (Nếu cần lưu dữ liệu)**
```
ESP32 → HiveMQ → Backend (Node.js) → SQLite
                      ↓
                  WebSocket
                      ↓
                  Frontend
```
- **Ưu điểm**: Lưu lịch sử, phân tích dữ liệu
- **Nhược điểm**: Phức tạp hơn (nhưng vẫn dễ làm)
- **Thời gian setup**: 1-2 giờ

---

## 🔌 REST API Specification (Backend Bridge)

> **Lưu ý**: Đây là API đơn giản cho dự án học tập - KHÔNG CẦN authentication

### Base URL
```
http://localhost:3000/api
```

---

### 📊 **Endpoints: Devices**

#### `GET /api/devices`
Lấy danh sách thiết bị.

**Response:**
```json
{
  "devices": [
    {
      "device_id": "power_monitor_01",
      "name": "Power Monitor 01",
      "online": true,
      "last_seen": "2025-12-18T10:30:00Z",
      "ip": "192.168.1.100",
      "rssi": -45
    }
  ]
}
```

#### `GET /api/devices/{deviceId}/status`
Lấy trạng thái thiết bị hiện tại.

**Response:**
```json
{
  "device_id": "power_monitor_01",
  "online": true,
  "ip": "192.168.1.100",
  "rssi": -45,
  "uptime": 3600,
  "free_heap": 245000,
  "channels": {
    "ch1": {
      "switch": "ON",
      "simulator": 0,
      "last_error": null
    },
    "ch2": {
      "switch": "OFF",
      "simulator": 50,
      "last_error": "OVERCURRENT"
    }
  },
  "last_update": "2025-12-18T10:30:00Z"
}
```

---

### 📈 **Endpoints: Telemetry**

#### `GET /api/devices/{deviceId}/telemetry/latest`
Lấy dữ liệu telemetry mới nhất.

**Response:**
```json
{
  "device_id": "power_monitor_01",
  "timestamp": 1702900800,
  "ch1": {
    "voltage": 12.05,
    "current": 1.234,
    "power": 14.87
  },
  "ch2": {
    "voltage": 11.98,
    "current": 0.567,
    "power": 6.79
  }
}
```

#### `GET /api/devices/{deviceId}/telemetry/history`
Lấy lịch sử telemetry.

**Query Parameters:**
- `from` (ISO 8601): Timestamp bắt đầu
- `to` (ISO 8601): Timestamp kết thúc
- `channel` (optional): `1`, `2`, hoặc `all` (default)
- `limit` (default: 100): Số record tối đa
- `interval` (optional): `1m`, `5m`, `1h` - downsample data

**Example:**
```
GET /api/devices/power_monitor_01/telemetry/history?from=2025-12-18T00:00:00Z&to=2025-12-18T23:59:59Z&channel=1&limit=500
```

**Response:**
```json
{
  "device_id": "power_monitor_01",
  "channel": 1,
  "from": "2025-12-18T00:00:00Z",
  "to": "2025-12-18T23:59:59Z",
  "count": 500,
  "data": [
    {
      "timestamp": 1702900800,
      "voltage": 12.05,
      "current": 1.234,
      "power": 14.87
    },
    {
      "timestamp": 1702900810,
      "voltage": 12.04,
      "current": 1.230,
      "power": 14.81
    }
  ]
}
```

---

### 🎛️ **Endpoints: Control**

#### `POST /api/devices/{deviceId}/channels/{ch}/switch`
Điều khiển công tắc kênh (ch: `1` hoặc `2`).

**Request Body:**
```json
{
  "action": "ON"
}
```
**Actions:** `ON`, `OFF`, `TOGGLE`

**Response:**
```json
{
  "success": true,
  "device_id": "power_monitor_01",
  "channel": 1,
  "action": "ON",
  "mqtt_topic": "devices/power_monitor_01/ch1/switch/set",
  "mqtt_payload": "ON",
  "timestamp": "2025-12-18T10:30:00Z"
}
```

**cURL Example:**
```bash
curl -X POST http://localhost:3000/api/devices/power_monitor_01/channels/1/switch \
  -H "Content-Type: application/json" \
  -d '{"action":"ON"}'
```

#### `POST /api/devices/{deviceId}/channels/{ch}/simulator`
Điều khiển simulator (giả lập lỗi).

**Request Body:**
```json
{
  "value": 75
}
```
**Value:** `0-100` (%)

**Response:**
```json
{
  "success": true,
  "device_id": "power_monitor_01",
  "channel": 2,
  "value": 75,
  "mqtt_topic": "devices/power_monitor_01/ch2/sim/set",
  "mqtt_payload": "75"
}
```

#### `POST /api/devices/{deviceId}/channels/{ch}/clear-error`
Xóa lỗi kênh.

**Response:**
```json
{
  "success": true,
  "device_id": "power_monitor_01",
  "channel": 1,
  "message": "Error cleared"
}
```

---

### ⚠️ **Endpoints: Errors**

#### `GET /api/devices/{deviceId}/errors`
Lấy lịch sử lỗi.

**Query Parameters:**
- `from`, `to` (ISO 8601): Khoảng thời gian
- `channel` (optional): `1`, `2`, hoặc `all`
- `severity` (optional): `WARNING`, `CRITICAL`
- `limit` (default: 50)

**Response:**
```json
{
  "device_id": "power_monitor_01",
  "count": 2,
  "errors": [
    {
      "id": "err_123456",
      "timestamp": "2025-12-18T10:25:30Z",
      "channel": 1,
      "error_type": "OVERCURRENT",
      "message": "Overcurrent: 3.65A",
      "value": 3.65,
      "severity": "CRITICAL",
      "action": "AUTO_SHUTDOWN",
      "cleared": false
    },
    {
      "id": "err_123455",
      "timestamp": "2025-12-18T09:15:22Z",
      "channel": 2,
      "error_type": "OVERVOLTAGE",
      "message": "Overvoltage: 14.5V",
      "value": 14.5,
      "severity": "CRITICAL",
      "action": "AUTO_SHUTDOWN",
      "cleared": true,
      "cleared_at": "2025-12-18T09:20:00Z"
    }
  ]
}
```

---

## 🔄 WebSocket API (Real-time Events)

### Connection
```javascript
const ws = new WebSocket('ws://localhost:3000/ws');
```

### Events từ Server → Client

#### 1. **telemetry** - Dữ liệu đo realtime
```json
{
  "event": "telemetry",
  "device_id": "power_monitor_01",
  "timestamp": 1702900800,
  "ch1": {"voltage": 12.05, "current": 1.234, "power": 14.87},
  "ch2": {"voltage": 11.98, "current": 0.567, "power": 6.79}
}
```

#### 2. **status** - Thay đổi trạng thái
```json
{
  "event": "status",
  "device_id": "power_monitor_01",
  "channel": 1,
  "switch": "ON",
  "simulator": 0,
  "timestamp": 1702900800
}
```

#### 3. **error** - Cảnh báo lỗi
```json
{
  "event": "error",
  "device_id": "power_monitor_01",
  "channel": 1,
  "error_type": "OVERCURRENT",
  "message": "Overcurrent: 3.65A",
  "value": 3.65,
  "severity": "CRITICAL",
  "action": "AUTO_SHUTDOWN",
  "timestamp": 1702900800
}
```

#### 4. **device_online** / **device_offline**
```json
{
  "event": "device_online",
  "device_id": "power_monitor_01",
  "ip": "192.168.1.100",
  "timestamp": 1702900800
}
```

#### 5. **heartbeat** - Heartbeat từ thiết bị
```json
{
  "event": "heartbeat",
  "device_id": "power_monitor_01",
  "uptime": 3600,
  "free_heap": 245000,
  "timestamp": 1702900800
}
```

### Messages từ Client → Server (optional)

Client có thể gửi lệnh điều khiển qua WebSocket:
```json
{
  "action": "control",
  "device_id": "power_monitor_01",
  "channel": 1,
  "command": "switch",
  "value": "ON"
}
```

---

## 💾 Database Schema

### Table: `devices`
```sql
CREATE TABLE devices (
    device_id VARCHAR(50) PRIMARY KEY,
    name VARCHAR(100),
    online BOOLEAN DEFAULT FALSE,
    last_seen TIMESTAMP,
    ip VARCHAR(45),
    rssi INTEGER,
    uptime INTEGER,
    free_heap INTEGER,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_devices_online ON devices(online);
CREATE INDEX idx_devices_last_seen ON devices(last_seen);
```

### Table: `telemetry`
```sql
CREATE TABLE telemetry (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    device_id VARCHAR(50) NOT NULL,
    channel TINYINT NOT NULL,  -- 1 hoặc 2
    voltage DECIMAL(6,3),
    current DECIMAL(6,3),
    power DECIMAL(8,3),
    timestamp BIGINT NOT NULL,  -- Unix timestamp (ms)
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    FOREIGN KEY (device_id) REFERENCES devices(device_id),
    INDEX idx_device_time (device_id, timestamp),
    INDEX idx_channel_time (device_id, channel, timestamp)
);

-- Partition theo timestamp (optional, cho DB lớn)
-- PARTITION BY RANGE (timestamp) (...)
```

### Table: `channel_status`
```sql
CREATE TABLE channel_status (
    device_id VARCHAR(50) NOT NULL,
    channel TINYINT NOT NULL,
    switch_state ENUM('ON', 'OFF') DEFAULT 'OFF',
    simulator_value TINYINT DEFAULT 0,  -- 0-100
    last_error VARCHAR(50),
    last_update TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    PRIMARY KEY (device_id, channel),
    FOREIGN KEY (device_id) REFERENCES devices(device_id)
);
```

### Table: `errors`
```sql
CREATE TABLE errors (
    id VARCHAR(50) PRIMARY KEY,
    device_id VARCHAR(50) NOT NULL,
    channel TINYINT,
    error_type VARCHAR(50) NOT NULL,
    message TEXT,
    value DECIMAL(10,3),
    severity ENUM('WARNING', 'CRITICAL') DEFAULT 'WARNING',
    action VARCHAR(50),
    cleared BOOLEAN DEFAULT FALSE,
    cleared_at TIMESTAMP NULL,
    timestamp TIMESTAMP NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    
    FOREIGN KEY (device_id) REFERENCES devices(device_id),
    INDEX idx_device_time (device_id, timestamp),
    INDEX idx_severity (severity, cleared)
);
```

### Data Retention Policy
```sql
-- Xóa telemetry cũ hơn 30 ngày (chạy daily)
DELETE FROM telemetry 
WHERE created_at < DATE_SUB(NOW(), INTERVAL 30 DAY);

-- Lưu aggregated data vào bảng riêng (hourly/daily summary)
CREATE TABLE telemetry_hourly (
    device_id VARCHAR(50),
    channel TINYINT,
    hour TIMESTAMP,
    avg_voltage DECIMAL(6,3),
    avg_current DECIMAL(6,3),
    avg_power DECIMAL(8,3),
    max_power DECIMAL(8,3),
    min_power DECIMAL(8,3),
    PRIMARY KEY (device_id, channel, hour)
);
```

---

## 💻 Code Examples

### 1. Frontend: MQTT.js (Direct Connection)

**Installation:**
```bash
npm install mqtt
```

**JavaScript/React Example:**
```javascript
import mqtt from 'mqtt';

// Kết nối MQTT over WebSocket (HiveMQ public - không cần auth)
const client = mqtt.connect('wss://broker.hivemq.com:8884/mqtt', {
  clientId: 'web_dashboard_' + Math.random().toString(16).substr(2, 8)
});

client.on('connect', () => {
  console.log('Connected to MQTT broker');
  
  // Subscribe các topic
  client.subscribe('devices/power_monitor_01/telemetry');
  client.subscribe('devices/power_monitor_01/+/status');
  client.subscribe('devices/power_monitor_01/error');
  client.subscribe('devices/power_monitor_01/heartbeat');
});

client.on('message', (topic, message) => {
  const payload = JSON.parse(message.toString());
  
  if (topic.includes('/telemetry')) {
    // Update UI với dữ liệu telemetry
    updateTelemetryDisplay(payload);
  } else if (topic.includes('/status')) {
    // Update UI với trạng thái
    updateStatusDisplay(payload);
  } else if (topic.includes('/error')) {
    // Hiển thị cảnh báo
    showErrorAlert(payload);
  }
});

// Điều khiển thiết bị
function controlSwitch(channel, action) {
  const topic = `devices/power_monitor_01/ch${channel}/switch/set`;
  client.publish(topic, action);  // 'ON', 'OFF', 'TOGGLE'
}

function setSimulator(channel, value) {
  const topic = `devices/power_monitor_01/ch${channel}/sim/set`;
  client.publish(topic, value.toString());  // '0' - '100'
}

// Sử dụng
controlSwitch(1, 'ON');
setSimulator(2, 50);
```

---

### 2. Frontend: WebSocket (Backend Bridge)

**JavaScript/Vue Example:**
```javascript
let ws;

function connectWebSocket() {
  ws = new WebSocket('ws://localhost:3000/ws');
  
  ws.onopen = () => {
    console.log('WebSocket connected');
    
    // Subscribe devices (optional)
    ws.send(JSON.stringify({
      action: 'subscribe',
      device_id: 'power_monitor_01'
    }));
  };
  
  ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    
    switch(data.event) {
      case 'telemetry':
        updateChart(data);
        updateGauges(data.ch1, data.ch2);
        break;
        
      case 'status':
        updateSwitchUI(data.channel, data.switch);
        updateSimulatorUI(data.channel, data.simulator);
        break;
        
      case 'error':
        showNotification('error', data.message);
        break;
        
      case 'device_online':
        setDeviceStatus('online');
        break;
        
      case 'device_offline':
        setDeviceStatus('offline');
        break;
    }
  };
  
  ws.onerror = (error) => {
    console.error('WebSocket error:', error);
  };
  
  ws.onclose = () => {
    console.log('WebSocket closed, reconnecting...');
    setTimeout(connectWebSocket, 5000);
  };
}

// Gửi lệnh điều khiển (optional - hoặc dùng REST API)
function sendControl(channel, command, value) {
  ws.send(JSON.stringify({
    action: 'control',
    device_id: 'power_monitor_01',
    channel: channel,
    command: command,  // 'switch', 'simulator'
    value: value       // 'ON'/'OFF' hoặc 0-100
  }));
}

connectWebSocket();
```

---

### 3. Backend: Node.js + Express + MQTT (Đơn giản)

**Installation:**
```bash
npm install express mqtt ws sqlite3
```

**server.js (Skeleton):**
```javascript
const express = require('express');
const mqtt = require('mqtt');
const WebSocket = require('ws');
const sqlite3 = require('sqlite3').verbose();

const app = express();
app.use(express.json());

// Database
const db = new sqlite3.Database('./power_monitor.db');

// MQTT Client
const mqttClient = mqtt.connect('mqtt://broker.hivemq.com:1883', {
  clientId: 'backend_server_' + Math.random().toString(16).substr(2, 8)
});

mqttClient.on('connect', () => {
  console.log('Backend connected to MQTT broker');
  mqttClient.subscribe('devices/+/telemetry');
  mqttClient.subscribe('devices/+/+/status');
  mqttClient.subscribe('devices/+/error');
  mqttClient.subscribe('devices/+/heartbeat');
});

// WebSocket Server
const wss = new WebSocket.Server({ noServer: true });
const clients = new Set();

// MQTT Message Handler
mqttClient.on('message', (topic, message) => {
  const payload = JSON.parse(message.toString());
  
  // Lưu vào database
  if (topic.includes('/telemetry')) {
    saveTelemetry(payload);
  } else if (topic.includes('/error')) {
    saveError(payload);
  }
  
  // Broadcast to WebSocket clients
  const wsMessage = {
    event: getEventType(topic),
    ...payload
  };
  
  clients.forEach(client => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(JSON.stringify(wsMessage));
    }
  });
});

// REST API: Get latest telemetry
app.get('/api/devices/:deviceId/telemetry/latest', (req, res) => {
  const { deviceId } = req.params;
  
  db.get(
    `SELECT * FROM telemetry 
     WHERE device_id = ? 
     ORDER BY timestamp DESC LIMIT 1`,
    [deviceId],
    (err, row) => {
      if (err) return res.status(500).json({ error: err.message });
      res.json(row || {});
    }
  );
});

// REST API: Control switch
app.post('/api/devices/:deviceId/channels/:ch/switch', (req, res) => {
  const { deviceId, ch } = req.params;
  const { action } = req.body;
  
  if (!['ON', 'OFF', 'TOGGLE'].includes(action)) {
    return res.status(400).json({ error: 'Invalid action' });
  }
  
  const topic = `devices/${deviceId}/ch${ch}/switch/set`;
  mqttClient.publish(topic, action);
  
  res.json({
    success: true,
    device_id: deviceId,
    channel: parseInt(ch),
    action: action,
    mqtt_topic: topic
  });
});

// REST API: Control simulator
app.post('/api/devices/:deviceId/channels/:ch/simulator', (req, res) => {
  const { deviceId, ch } = req.params;
  const { value } = req.body;
  
  if (value < 0 || value > 100) {
    return res.status(400).json({ error: 'Value must be 0-100' });
  }
  
  const topic = `devices/${deviceId}/ch${ch}/sim/set`;
  mqttClient.publish(topic, value.toString());
  
  res.json({
    success: true,
    device_id: deviceId,
    channel: parseInt(ch),
    value: value,
    mqtt_topic: topic
  });
});

// WebSocket upgrade
const server = app.listen(3000, () => {
  console.log('Server running on port 3000');
});

server.on('upgrade', (request, socket, head) => {
  wss.handleUpgrade(request, socket, head, (ws) => {
    wss.emit('connection', ws, request);
  });
});

wss.on('connection', (ws) => {
  clients.add(ws);
  console.log('WebSocket client connected');
  
  ws.on('close', () => {
    clients.delete(ws);
    console.log('WebSocket client disconnected');
  });
});

// Helper functions
function saveTelemetry(payload) {
  const { device_id, timestamp, ch1, ch2 } = payload;
  
  // Save ch1
  if (ch1) {
    db.run(
      `INSERT INTO telemetry (device_id, channel, voltage, current, power, timestamp)
       VALUES (?, 1, ?, ?, ?, ?)`,
      [device_id, ch1.voltage, ch1.current, ch1.power, timestamp]
    );
  }
  
  // Save ch2
  if (ch2) {
    db.run(
      `INSERT INTO telemetry (device_id, channel, voltage, current, power, timestamp)
       VALUES (?, 2, ?, ?, ?, ?)`,
      [device_id, ch2.voltage, ch2.current, ch2.power, timestamp]
    );
  }
}

function saveError(payload) {
  db.run(
    `INSERT INTO errors (id, device_id, channel, error_type, message, value, severity, action, timestamp)
     VALUES (?, ?, ?, ?, ?, ?, ?, ?, datetime('now'))`,
    [
      'err_' + Date.now(),
      payload.device_id,
      payload.channel,
      payload.error_type,
      payload.message,
      payload.value,
      payload.severity,
      payload.action
    ]
  );
}

function getEventType(topic) {
  if (topic.includes('/telemetry')) return 'telemetry';
  if (topic.includes('/status')) return 'status';
  if (topic.includes('/error')) return 'error';
  if (topic.includes('/heartbeat')) return 'heartbeat';
  return 'unknown';
}
```

---

## 🔒 Bảo mật (Dành cho Dự án Học tập)

### ✅ Đủ dùng cho demo/học tập:

**1. Chạy trên mạng LAN/Localhost**
```javascript
// Backend chỉ listen trên localhost hoặc LAN
app.listen(3000, 'localhost', () => {
  console.log('Server chạy trên http://localhost:3000');
});
```

**2. CORS đơn giản (cho phép tất cả - chỉ dùng học tập)**
```bash
npm install cors
```
```javascript
const cors = require('cors');
app.use(cors());  // Cho phép tất cả origins
```

**3. Input Validation cơ bản**
```javascript
// Validate control commands
function validateSwitchAction(action) {
  return ['ON', 'OFF', 'TOGGLE'].includes(action);
}

function validateSimulatorValue(value) {
  return typeof value === 'number' && value >= 0 && value <= 100;
}

// Sử dụng trong API
app.post('/api/devices/:deviceId/channels/:ch/switch', (req, res) => {
  const { action } = req.body;
  
  if (!validateSwitchAction(action)) {
    return res.status(400).json({ error: 'Invalid action' });
  }
  
  // ... xử lý tiếp
});
```

**4. HiveMQ Public Broker**
```javascript
// Sử dụng HiveMQ public (không cần auth)
const mqttClient = mqtt.connect('mqtt://broker.hivemq.com:1883', {
  clientId: 'backend_' + Math.random().toString(16).substr(2, 8)
});
```

### 🚫 KHÔNG CẦN cho dự án học tập:
- ❌ JWT Authentication
- ❌ Rate Limiting
- ❌ HTTPS/TLS (chỉ cần nếu deploy public)
- ❌ Database encryption
- ❌ MQTT ACL

### ⚠️ Lưu ý:
- Đừng deploy setup này lên Internet công khai
- Chỉ dùng trong mạng LAN hoặc localhost
- Nếu cần demo qua Internet, dùng ngrok với password

---

## 📊 UI Components Recommendation

### Dashboard Layout
```
┌─────────────────────────────────────────────────┐
│  Header: Device Status | Last Update: 10:30:00  │
├─────────────────────────────────────────────────┤
│  ┌──────────┐  ┌──────────┐                     │
│  │  CH1     │  │  CH2     │   [Realtime Chart]  │
│  │ 12.05V   │  │ 11.98V   │                     │
│  │  1.23A   │  │  0.57A   │   Voltage/Current   │
│  │ 14.87W   │  │  6.79W   │   Power over time   │
│  │          │  │          │                     │
│  │ [ON/OFF] │  │ [ON/OFF] │                     │
│  │ Sim: 0%  │  │ Sim: 50% │                     │
│  └──────────┘  └──────────┘                     │
├─────────────────────────────────────────────────┤
│  Recent Errors:                                  │
│  ⚠ CH1: Overcurrent 3.65A - 10:25:30            │
│  ⚠ CH2: Overvoltage 14.5V - 09:15:22 [CLEARED]  │
└─────────────────────────────────────────────────┘
```

### Recommended Libraries
- **Charts**: Chart.js, Apache ECharts, Recharts
- **Gauges**: canvas-gauges, react-gauge-chart
- **UI Framework**: React + Material-UI, Vue + Vuetify, Angular + Angular Material
- **State Management**: Redux, Vuex, NgRx
- **Real-time**: MQTT.js, Socket.io-client

---

## 🚀 Setup Checklist (Dự án Học tập)

### ✅ ESP32 Firmware (BẮT BUỘC)
- [ ] Cấu hình WiFi SSID/password trong `config.h`
- [ ] Cấu hình MQTT broker: `broker.hivemq.com`
- [ ] Upload firmware lên ESP32
- [ ] Mở Serial Monitor, check "Connected to MQTT broker"
- [ ] Test gửi lệnh qua Serial: `on1`, `off1`

### 🌐 Option A: Frontend MQTT trực tiếp (10 phút)
- [ ] Tạo file `index.html` với code ở phần Quick Start
- [ ] Mở file HTML trong browser
- [ ] Mở Console (F12) xem logs
- [ ] Test nhận data và điều khiển
- **XONG!** 🎉

### 🗄️ Option B: Backend + Database (nếu cần lưu data)
- [ ] Cài Node.js
- [ ] `npm install express mqtt ws sqlite3`
- [ ] Copy code backend từ README
- [ ] Tạo database SQLite (tự động khi chạy)
- [ ] `node server.js`
- [ ] Test API: `curl http://localhost:3000/api/devices/power_monitor_01/telemetry/latest`
- [ ] Tạo frontend kết nối WebSocket

### 🧪 Testing Cơ bản
- [ ] ESP32 publish telemetry mỗi 5s
- [ ] Dashboard hiển thị data realtime
- [ ] Bật/tắt CH1, CH2 từ dashboard
- [ ] Điều chỉnh simulator 0-100%
- [ ] Kiểm tra error khi overcurrent/overvoltage

### 📱 Demo/Trình bày
- [ ] Chuẩn bị laptop + ESP32 + nguồn
- [ ] Kết nối WiFi ổn định
- [ ] Mở dashboard trên laptop
- [ ] (Optional) Dùng ngrok để demo từ xa: `ngrok http 3000`

---

## 📞 Liên hệ

Nếu gặp vấn đề, hãy mở issue hoặc liên hệ qua email.
