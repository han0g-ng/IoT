# INTEGRATION GUIDE
## Hướng dẫn tích hợp Backend + Database + Web Dashboard

---

## 🎯 Mục tiêu

Xây dựng hệ thống để:
1. **Lưu trữ dữ liệu**: Ghi telemetry từ ESP32 vào database
2. **Hiển thị real-time**: Web dashboard hiển thị dữ liệu trực tiếp
3. **Điều khiển thiết bị**: Bật/tắt đèn 1, đèn 2 qua giao diện web
4. **Mô phỏng lỗi**: Điều chỉnh simulator qua slider

---

## 🏗️ Kiến trúc hệ thống

```
┌─────────────┐
│   ESP32     │ ──publish──> ┌──────────────────┐
│  (Hardware) │              │  MQTT Broker     │
└─────────────┘ <─subscribe─ │ broker.hivemq.com│
                              └──────────────────┘
                                      ↑ ↓
                                 subscribe/publish
                                      ↑ ↓
                              ┌──────────────────┐
                              │  Backend Server  │
                              │   (Node.js)      │
                              │                  │
                              │ • MQTT Client    │
                              │ • REST API       │
                              │ • WebSocket      │
                              └──────────────────┘
                                   ↑ ↓        ↑ ↓
                            REST API ↑        ↑ WebSocket
                                   ↑ ↓        ↑ ↓
                    ┌──────────────────┐  ┌──────────────────┐
                    │    Database      │  │  Web Frontend    │
                    │  (MySQL/Postgres)│  │  (HTML/JS/CSS)   │
                    │                  │  │                  │
                    │ • telemetry      │  │ • Charts         │
                    │ • device_status  │  │ • Controls       │
                    │ • system_health  │  │ • Real-time UI   │
                    └──────────────────┘  └──────────────────┘
```

---

## 📋 Checklist công việc

### PHASE 1: Setup môi trường (30 phút)
- [ ] Cài đặt Node.js (v18+)
- [ ] Cài đặt MySQL/PostgreSQL
- [ ] Tạo project folder: `iot_backend`
- [ ] Init npm project: `npm init -y`
- [ ] Cài packages cần thiết (xem bên dưới)

### PHASE 2: Database Setup (30 phút)
- [ ] Tạo database: `iot_power_monitor`
- [ ] Tạo bảng `devices`
- [ ] Tạo bảng `telemetry`
- [ ] Tạo bảng `device_status`
- [ ] Tạo bảng `system_health`
- [ ] Test kết nối database

### PHASE 3: Backend MQTT Integration (1-2 giờ)
- [ ] Tạo file `mqtt_client.js`
- [ ] Kết nối tới broker.hivemq.com
- [ ] Subscribe topic: `devices/anh_hong_dep_trai_ittn/#`
- [ ] Parse JSON messages
- [ ] Lưu telemetry vào database
- [ ] Lưu status vào database
- [ ] Test bằng cách chạy ESP32 và xem data vào DB

### PHASE 4: REST API (1-2 giờ)
- [ ] Tạo file `server.js` (Express)
- [ ] API GET `/api/devices` - Danh sách thiết bị
- [ ] API GET `/api/telemetry/:channel` - Lấy dữ liệu đo đạc
- [ ] API GET `/api/telemetry/:channel/latest` - Dữ liệu mới nhất
- [ ] API GET `/api/status/:channel` - Trạng thái hiện tại
- [ ] API POST `/api/control/:channel/switch` - Bật/tắt
- [ ] API POST `/api/control/:channel/simulator` - Mô phỏng lỗi
- [ ] Test API bằng Postman/Thunder Client

### PHASE 5: WebSocket Real-time (1 giờ)
- [ ] Thêm Socket.IO vào server
- [ ] Emit event khi nhận MQTT message mới
- [ ] Emit `telemetry` event
- [ ] Emit `status` event
- [ ] Emit `heartbeat` event

### PHASE 6: Web Frontend (2-3 giờ)
- [ ] Tạo folder `public/`
- [ ] HTML: Layout cơ bản
- [ ] CSS: Styling dashboard
- [ ] JavaScript: Kết nối WebSocket
- [ ] Display real-time telemetry (V, I, P)
- [ ] Nút ON/OFF cho CH1, CH2
- [ ] Slider cho simulator (0-100%)
- [ ] Chart.js: Vẽ biểu đồ lịch sử

### PHASE 7: Testing & Demo (1 giờ)
- [ ] Test bật ESP32 → Thấy data trên web
- [ ] Test click nút ON/OFF → ESP32 phản ứng
- [ ] Test simulator slider → Thấy current/power thay đổi
- [ ] Chuẩn bị demo presentation

---

## 📦 NPM Packages cần cài

```bash
npm install express
npm install mqtt
npm install mysql2       # hoặc pg (PostgreSQL)
npm install socket.io
npm install cors
npm install dotenv
npm install body-parser
```

### package.json mẫu
```json
{
  "name": "iot-power-monitor-backend",
  "version": "1.0.0",
  "description": "IoT Power Monitor Backend",
  "main": "server.js",
  "scripts": {
    "start": "node server.js",
    "dev": "nodemon server.js"
  },
  "dependencies": {
    "express": "^4.18.2",
    "mqtt": "^5.3.0",
    "mysql2": "^3.6.5",
    "socket.io": "^4.6.0",
    "cors": "^2.8.5",
    "dotenv": "^16.3.1",
    "body-parser": "^1.20.2"
  },
  "devDependencies": {
    "nodemon": "^3.0.2"
  }
}
```

---

## 🗄️ Database Schema (Chi tiết)

Xem file `MQTT_API_DOCUMENTATION.md` phần Database Schema.

Script SQL tạo bảng (MySQL):
```sql
-- Create database
CREATE DATABASE iot_power_monitor CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
USE iot_power_monitor;

-- Devices table
CREATE TABLE devices (
    id VARCHAR(50) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    firmware VARCHAR(20),
    ip VARCHAR(15),
    last_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Telemetry table (main data storage)
CREATE TABLE telemetry (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50) NOT NULL,
    channel TINYINT NOT NULL,
    voltage DECIMAL(6,3) NOT NULL,
    current DECIMAL(8,4) NOT NULL,
    power DECIMAL(8,3) NOT NULL,
    esp_timestamp BIGINT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_device_channel (device_id, channel),
    INDEX idx_created_at (created_at),
    FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE
);

-- Device status table
CREATE TABLE device_status (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50) NOT NULL,
    channel TINYINT NOT NULL,
    switch_state BOOLEAN NOT NULL,
    simulator_value TINYINT NOT NULL,
    esp_timestamp BIGINT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_device_channel (device_id, channel),
    FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE
);

-- System health table
CREATE TABLE system_health (
    id INT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50) NOT NULL,
    uptime INT,
    free_heap INT,
    wifi_rssi INT,
    esp_timestamp BIGINT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_device (device_id),
    FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE CASCADE
);

-- Insert device record
INSERT INTO devices (id, name, firmware, ip) 
VALUES ('anh_hong_dep_trai_ittn', 'ESP32 Power Monitor', '1.0.0', '192.168.1.2');
```

---

## 🔌 Backend Code Structure

```
iot_backend/
├── server.js              # Main Express server + API routes
├── mqtt_client.js         # MQTT subscriber & database writer
├── database.js            # Database connection & queries
├── config.js              # Configuration (MQTT broker, DB credentials)
├── package.json
├── .env                   # Environment variables
└── public/                # Static files
    ├── index.html         # Dashboard UI
    ├── style.css          # Styling
    └── app.js             # Frontend JavaScript
```

---

## 🔧 Backend Logic Flow

### 1. MQTT Client (mqtt_client.js)

**Nhiệm vụ:**
- Kết nối tới MQTT broker
- Subscribe topic `devices/anh_hong_dep_trai_ittn/#`
- Nhận message và phân loại
- Lưu vào database
- Emit WebSocket event

**Pseudo Code:**
```javascript
const mqtt = require('mqtt');
const db = require('./database');
const io = require('./server').io; // Socket.IO instance

// Connect to MQTT broker
const client = mqtt.connect('mqtt://broker.hivemq.com:1883');

client.on('connect', () => {
    console.log('Connected to MQTT broker');
    client.subscribe('devices/anh_hong_dep_trai_ittn/#');
});

client.on('message', (topic, message) => {
    const data = JSON.parse(message.toString());
    
    // Route based on topic
    if (topic.endsWith('/telemetry')) {
        handleTelemetry(topic, data);
    } else if (topic.endsWith('/status')) {
        handleStatus(topic, data);
    } else if (topic.endsWith('/heartbeat')) {
        handleHeartbeat(topic, data);
    }
});

function handleTelemetry(topic, data) {
    // Parse topic to get channel
    const channel = topic.includes('/ch1/') ? 1 : 
                    topic.includes('/ch2/') ? 2 : null;
    
    if (channel) {
        // Save to database
        db.saveTelemetry({
            device_id: 'anh_hong_dep_trai_ittn',
            channel: channel,
            voltage: data.voltage,
            current: data.current,
            power: data.power,
            esp_timestamp: data.timestamp
        });
        
        // Broadcast to WebSocket clients
        io.emit('telemetry', {
            channel: channel,
            data: data
        });
    } else if (topic.endsWith('/telemetry')) {
        // Combined telemetry
        io.emit('telemetry_combined', data);
    }
}

function handleStatus(topic, data) {
    // Similar logic for status messages
    // Save to device_status table
    // Emit WebSocket event
}

function handleHeartbeat(topic, data) {
    // Save to system_health table
    // Update devices.last_seen
    // Emit WebSocket event
}

// Export publish function for API
function publishControl(channel, command, value) {
    const topic = `devices/anh_hong_dep_trai_ittn/ch${channel}/${command}/set`;
    client.publish(topic, value);
}

module.exports = { publishControl };
```

---

### 2. REST API (server.js)

**Endpoint List:**

#### GET `/api/devices`
Lấy danh sách thiết bị
```json
{
  "success": true,
  "data": [
    {
      "id": "anh_hong_dep_trai_ittn",
      "name": "ESP32 Power Monitor",
      "firmware": "1.0.0",
      "ip": "192.168.1.2",
      "last_seen": "2025-12-20T09:45:30.000Z"
    }
  ]
}
```

#### GET `/api/telemetry/:channel/latest`
Lấy dữ liệu mới nhất của channel
```json
{
  "success": true,
  "data": {
    "channel": 1,
    "voltage": 12.219,
    "current": 0.0003,
    "power": 0.004,
    "timestamp": "2025-12-20T09:45:30.000Z"
  }
}
```

#### GET `/api/telemetry/:channel/history?limit=100`
Lấy lịch sử dữ liệu
```json
{
  "success": true,
  "data": [
    {
      "voltage": 12.219,
      "current": 0.0003,
      "power": 0.004,
      "created_at": "2025-12-20T09:45:30.000Z"
    },
    ...
  ]
}
```

#### POST `/api/control/:channel/switch`
Điều khiển bật/tắt
**Request Body:**
```json
{
  "state": "ON"
}
```
**Response:**
```json
{
  "success": true,
  "message": "Switch command sent",
  "channel": 1,
  "state": "ON"
}
```

#### POST `/api/control/:channel/simulator`
Mô phỏng lỗi
**Request Body:**
```json
{
  "value": 50
}
```
**Response:**
```json
{
  "success": true,
  "message": "Simulator value set",
  "channel": 1,
  "value": 50
}
```

---

### 3. WebSocket Events

**Client → Server:** (Không cần, API REST đủ)

**Server → Client:**

#### Event: `telemetry`
```javascript
{
  "channel": 1,
  "voltage": 12.219,
  "current": 0.0003,
  "power": 0.004,
  "timestamp": 1126736
}
```

#### Event: `status`
```javascript
{
  "channel": 1,
  "switch": "ON",
  "switch_state": true,
  "simulator": 100
}
```

#### Event: `heartbeat`
```javascript
{
  "device_id": "anh_hong_dep_trai_ittn",
  "uptime": 1262,
  "free_heap": 247364,
  "wifi_rssi": -39
}
```

---

## 🎨 Web Dashboard Requirements

### Layout Suggestions

```
┌─────────────────────────────────────────────────────┐
│  ESP32 Power Monitor Dashboard                      │
├─────────────────────────────────────────────────────┤
│  Device: anh_hong_dep_trai_ittn                     │
│  Status: ● Online  |  IP: 192.168.1.2  |  RSSI: -39 │
├───────────────────────┬─────────────────────────────┤
│  CHANNEL 1 (LIGHT 1)  │  CHANNEL 2 (LIGHT 2)        │
├───────────────────────┼─────────────────────────────┤
│  ⚡ 12.22V            │  ⚡ 12.21V                   │
│  ↯  0.0003A           │  ↯  0.0001A                  │
│  ⚙  0.004W            │  ⚙  0.001W                   │
│                       │                              │
│  [●OFF] [○ON]         │  [○OFF] [●ON]                │
│                       │                              │
│  Simulator: 100%      │  Simulator: 100%             │
│  ├────────────┤ 100%  │  ├────────────┤ 100%         │
│  [Normal]             │  [Normal]                    │
├───────────────────────┴─────────────────────────────┤
│  📊 POWER HISTORY (Last 1 hour)                     │
│  ┌─────────────────────────────────────────────┐    │
│  │         Chart.js Line Chart                 │    │
│  │   (Voltage, Current, Power over time)       │    │
│  └─────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────┘
```

### UI Components Needed

1. **Status Bar**
   - Device online/offline indicator
   - IP address
   - WiFi signal strength
   - Last update timestamp

2. **Channel Cards (×2)**
   - Real-time voltage display
   - Real-time current display
   - Real-time power display
   - ON/OFF toggle buttons
   - Simulator slider (0-100%)
   - Quick preset buttons (Normal, 50% drop, Open circuit)

3. **Charts**
   - Line chart: Voltage over time
   - Line chart: Current over time
   - Line chart: Power over time
   - Time range selector (1h, 6h, 24h)

4. **System Info**
   - Uptime counter
   - Free memory
   - Connection status

---

## 🧪 Testing Strategy

### Test Scenario 1: Data Logging
1. Bật ESP32
2. Mở MySQL/PostgreSQL client
3. Chạy: `SELECT * FROM telemetry ORDER BY id DESC LIMIT 10;`
4. **Expected**: Thấy dữ liệu mới mỗi giây

### Test Scenario 2: Real-time Display
1. Mở web dashboard
2. Quan sát số liệu
3. **Expected**: Số liệu update mỗi giây

### Test Scenario 3: Control
1. Click nút "ON" cho Channel 1
2. Quan sát ESP32 (đèn sáng)
3. Xem MQTT monitor
4. **Expected**: Status message update

### Test Scenario 4: Simulator
1. Kéo slider simulator về 50%
2. Quan sát current/power giảm
3. **Expected**: Telemetry thay đổi theo tỷ lệ

### Test Scenario 5: Chart Display
1. Để ESP32 chạy 5 phút
2. Mở chart tab
3. **Expected**: Thấy đường cong V, I, P

---

## 📊 Data Flow Examples

### Example 1: Telemetry Flow
```
ESP32 reads INA226 → 12.22V, 0.0003A
       ↓
ESP32 publishes MQTT → devices/.../ch1/telemetry
       ↓
Backend MQTT client receives
       ↓
Parse JSON: {voltage: 12.22, current: 0.0003, power: 0.004}
       ↓
Save to DB: INSERT INTO telemetry (device_id, channel, voltage, ...)
       ↓
Emit WebSocket: io.emit('telemetry', data)
       ↓
Frontend receives → Update DOM elements
       ↓
User sees: ⚡ 12.22V displayed
```

### Example 2: Control Flow
```
User clicks "ON" button
       ↓
Frontend sends: POST /api/control/1/switch {state: "ON"}
       ↓
Backend API receives
       ↓
Publish MQTT: devices/.../ch1/switch/set → "ON"
       ↓
ESP32 receives → Turns on MOSFET
       ↓
ESP32 publishes status → {switch: "ON", switch_state: true}
       ↓
Backend receives → Emit WebSocket
       ↓
Frontend updates button state to active
```

---

## ⚙️ Configuration Files

### .env file
```env
# MQTT Configuration
MQTT_BROKER=broker.hivemq.com
MQTT_PORT=1883
MQTT_TOPIC=devices/anh_hong_dep_trai_ittn/#

# Database Configuration
DB_HOST=localhost
DB_USER=root
DB_PASSWORD=your_password
DB_NAME=iot_power_monitor

# Server Configuration
PORT=3000
```

---

## 🚀 Quick Start Commands

```bash
# 1. Clone/Create project
mkdir iot_backend
cd iot_backend

# 2. Initialize npm
npm init -y

# 3. Install dependencies
npm install express mqtt mysql2 socket.io cors dotenv body-parser

# 4. Create database
mysql -u root -p < database_schema.sql

# 5. Create .env file
echo "DB_PASSWORD=your_password" > .env

# 6. Start server
node server.js

# 7. Open browser
http://localhost:3000
```

---

## 📞 Support & Resources

### Documentation References
- MQTT Protocol: https://mqtt.org/
- Node.js MQTT Client: https://github.com/mqttjs/MQTT.js
- Socket.IO: https://socket.io/docs/v4/
- Chart.js: https://www.chartjs.org/docs/latest/
- Express.js: https://expressjs.com/

### Test Tools
- MQTT Explorer: https://mqtt-explorer.com/
- Postman: https://www.postman.com/
- MySQL Workbench: https://www.mysql.com/products/workbench/

### Provided Python Scripts
- `mqtt_monitor.py` - Monitor MQTT messages
- `test_control.py` - Send control commands
- `test_simulator.py` - Test fault simulation

---

## 💡 Tips & Best Practices

1. **Database Optimization**
   - Index trên (device_id, channel, created_at)
   - Partition table theo tháng nếu data lớn
   - Auto-delete data cũ hơn 30 ngày

2. **Error Handling**
   - Validate MQTT message format
   - Handle database connection loss
   - WebSocket reconnection logic

3. **Security**
   - Sanitize user input
   - Rate limiting cho API
   - CORS configuration
   - Không hardcode credentials

4. **Performance**
   - Cache latest telemetry in Redis
   - Debounce WebSocket emissions
   - Paginate API responses
   - Use connection pooling

---

## 🎯 Success Criteria

Hệ thống hoàn thành khi:
- ✅ Data từ ESP32 lưu vào database thành công
- ✅ Web dashboard hiển thị real-time (<2s delay)
- ✅ Nút ON/OFF điều khiển được ESP32
- ✅ Simulator slider hoạt động, thấy current/power thay đổi
- ✅ Chart hiển thị lịch sử ít nhất 1 giờ
- ✅ Không có lỗi console trong quá trình demo

**Timeline ước tính**: 6-8 giờ cho người có kinh nghiệm Node.js/MySQL

Good luck! 🚀
