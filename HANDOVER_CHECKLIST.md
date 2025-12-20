# HANDOVER CHECKLIST
## Bàn giao từ Team Hardware sang Team Web/Database

---

## ✅ ĐÃ HOÀN THÀNH (Hardware Team)

### 1. ESP32 Firmware
- ✅ Code ESP32 hoạt động ổn định
- ✅ WiFi kết nối tự động (SSID: B2306)
- ✅ MQTT kết nối broker.hivemq.com
- ✅ Đọc cảm biến INA226 (2 kênh)
- ✅ Điều khiển MOSFET (2 kênh)
- ✅ Mô phỏng lỗi nguồn (PWM 0-100%)
- ✅ Gửi telemetry mỗi 1 giây
- ✅ Gửi status mỗi 5 giây
- ✅ Gửi heartbeat mỗi 60 giây

### 2. Phần cứng
- ✅ ESP32-D0WD-V3 hoạt động
- ✅ INA226 CH1 (0x40): Đo đèn 1
- ✅ INA226 CH2 (0x41): Đo đèn 2
- ✅ MOSFET CH1 (GPIO 25/23): Điều khiển đèn 1
- ✅ MOSFET CH2 (GPIO 26/19): Điều khiển đèn 2
- ✅ Nguồn 12V cho thiết bị
- ✅ Nguồn 5V cho ESP32

### 3. MQTT Topics
- ✅ Telemetry: `devices/anh_hong_dep_trai_ittn/telemetry`
- ✅ CH1 Telemetry: `devices/anh_hong_dep_trai_ittn/ch1/telemetry`
- ✅ CH2 Telemetry: `devices/anh_hong_dep_trai_ittn/ch2/telemetry`
- ✅ CH1 Status: `devices/anh_hong_dep_trai_ittn/ch1/status`
- ✅ CH2 Status: `devices/anh_hong_dep_trai_ittn/ch2/status`
- ✅ Device Status: `devices/anh_hong_dep_trai_ittn/status`
- ✅ Heartbeat: `devices/anh_hong_dep_trai_ittn/heartbeat`
- ✅ CH1 Switch Control: `devices/anh_hong_dep_trai_ittn/ch1/switch/set`
- ✅ CH2 Switch Control: `devices/anh_hong_dep_trai_ittn/ch2/switch/set`
- ✅ CH1 Simulator: `devices/anh_hong_dep_trai_ittn/ch1/sim/set`
- ✅ CH2 Simulator: `devices/anh_hong_dep_trai_ittn/ch2/sim/set`

### 4. Test Scripts (Python)
- ✅ `mqtt_monitor.py` - Monitor tất cả messages
- ✅ `test_control.py` - Test điều khiển ON/OFF
- ✅ `test_simulator.py` - Test mô phỏng lỗi

### 5. Tài liệu
- ✅ `MQTT_API_DOCUMENTATION.md` - API Reference đầy đủ
- ✅ `INTEGRATION_GUIDE.md` - Hướng dẫn tích hợp
- ✅ `README.md` - Tài liệu firmware
- ✅ File checklist này

---

## 📦 BÀN GIAO CHO WEB/DATABASE TEAM

### Thông tin kết nối
- **MQTT Broker**: `broker.hivemq.com`
- **MQTT Port**: `1883`
- **Device ID**: `anh_hong_dep_trai_ittn`
- **WiFi Network**: B2306 (192.168.1.x)
- **ESP32 IP**: 192.168.1.2 (có thể thay đổi)

### Tài liệu cần đọc
1. **MQTT_API_DOCUMENTATION.md** - ĐỌC ĐẦU TIÊN
   - Danh sách topics
   - Format JSON
   - Database schema
   - Test examples

2. **INTEGRATION_GUIDE.md** - HƯỚNG DẪN THỰC HIỆN
   - Checklist từng bước
   - Code structure
   - API endpoints
   - WebSocket events
   - Testing strategy

3. **esp32_firmware/README.md** - Thông tin firmware
   - Pinout
   - Configuration
   - Build instructions

### Files có sẵn
```
D:\HUST\IoT\Project\
├── MQTT_API_DOCUMENTATION.md      ← ĐỌC ĐẦU TIÊN
├── INTEGRATION_GUIDE.md            ← HƯỚNG DẪN CHI TIẾT
├── HANDOVER_CHECKLIST.md           ← File này
└── esp32_firmware/
    ├── mqtt_monitor.py             ← Test tool
    ├── test_control.py             ← Test control
    ├── test_simulator.py           ← Test simulator
    └── README.md
```

---

## 🎯 NHIỆM VỤ CHO WEB/DATABASE TEAM

### Mục tiêu Demo
Tạo hệ thống có 2 chức năng chính:

#### 1. Giao diện Web - Hiển thị & Điều khiển Real-time
- [ ] Hiển thị telemetry real-time (V, I, P)
- [ ] Nút bật/tắt đèn 1 (CH1)
- [ ] Nút bật/tắt đèn 2 (CH2)
- [ ] Slider mô phỏng lỗi CH1 (0-100%)
- [ ] Slider mô phỏng lỗi CH2 (0-100%)
- [ ] Biểu đồ lịch sử (Chart.js)
- [ ] Trạng thái kết nối thiết bị

#### 2. Database - Lưu trữ dữ liệu đo đạc
- [ ] Lưu telemetry (V, I, P) mỗi giây
- [ ] Lưu status (switch, simulator)
- [ ] Lưu heartbeat (uptime, memory, signal)
- [ ] API lấy dữ liệu lịch sử
- [ ] Data retention (xóa data cũ)

---

## 🛠️ CÔNG NGHỆ ĐỀ XUẤT

### Backend
- **Node.js** + Express (REST API)
- **MQTT.js** (MQTT client)
- **Socket.IO** (WebSocket cho real-time)
- **MySQL** hoặc **PostgreSQL** (Database)

### Frontend
- **HTML/CSS/JavaScript** (hoặc React/Vue nếu muốn)
- **Chart.js** (Biểu đồ)
- **Socket.IO Client** (Real-time updates)
- **Bootstrap** hoặc **Tailwind CSS** (UI framework)

### Alternative (Nếu thích Python)
- **Python** + Flask/FastAPI
- **paho-mqtt** (MQTT client)
- **SQLAlchemy** (ORM)
- **Flask-SocketIO** (WebSocket)

---

## 📋 CHECKLIST TRIỂN KHAI

### Phase 1: Setup (30 phút)
- [ ] Cài Node.js v18+
- [ ] Cài MySQL/PostgreSQL
- [ ] Clone/tạo project folder
- [ ] `npm init` và install packages
- [ ] Test kết nối database

### Phase 2: Database (30 phút)
- [ ] Tạo database `iot_power_monitor`
- [ ] Chạy schema SQL (xem MQTT_API_DOCUMENTATION.md)
- [ ] Insert device record
- [ ] Test query

### Phase 3: MQTT Integration (1-2 giờ)
- [ ] Tạo MQTT client
- [ ] Subscribe topic
- [ ] Parse JSON messages
- [ ] Lưu vào database
- [ ] Test bằng mqtt_monitor.py

### Phase 4: REST API (1-2 giờ)
- [ ] Setup Express server
- [ ] GET `/api/devices`
- [ ] GET `/api/telemetry/:channel/latest`
- [ ] GET `/api/telemetry/:channel/history`
- [ ] POST `/api/control/:channel/switch`
- [ ] POST `/api/control/:channel/simulator`
- [ ] Test bằng Postman

### Phase 5: WebSocket (1 giờ)
- [ ] Setup Socket.IO
- [ ] Emit `telemetry` event
- [ ] Emit `status` event
- [ ] Emit `heartbeat` event
- [ ] Test connection

### Phase 6: Web Frontend (2-3 giờ)
- [ ] HTML layout
- [ ] CSS styling
- [ ] WebSocket connection
- [ ] Display real-time data
- [ ] ON/OFF buttons
- [ ] Simulator sliders
- [ ] Chart.js integration

### Phase 7: Testing & Demo (1 giờ)
- [ ] End-to-end test
- [ ] Demo script preparation
- [ ] Performance check
- [ ] Bug fixing

**Tổng thời gian ước tính**: 6-8 giờ

---

## 🧪 TESTING TOOLS

### Sẵn có
1. **mqtt_monitor.py**
   ```bash
   cd esp32_firmware
   python mqtt_monitor.py
   ```
   → Hiển thị tất cả MQTT messages

2. **test_control.py**
   ```bash
   python test_control.py
   ```
   → Test bật/tắt CH1, CH2

3. **test_simulator.py**
   ```bash
   python test_simulator.py
   ```
   → Test mô phỏng lỗi trên CH1

### Cần cài thêm
- **MQTT Explorer**: https://mqtt-explorer.com/
- **Postman**: https://www.postman.com/
- **MySQL Workbench**: (nếu dùng MySQL)

---

## 📊 DỮ LIỆU MẪU

### Telemetry Message Example
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

### Status Message Example
```json
{
  "channel": 1,
  "switch": "OFF",
  "switch_state": false,
  "simulator": 100,
  "timestamp": 1123195
}
```

### Control Command Example
Topic: `devices/anh_hong_dep_trai_ittn/ch1/switch/set`
Payload: `ON`

---

## 🚨 LƯU Ý QUAN TRỌNG

### 1. ESP32 phải chạy
- Đảm bảo ESP32 được cấp nguồn
- Kiểm tra serial monitor để thấy "System Ready!"
- Xác nhận kết nối WiFi và MQTT thành công

### 2. MQTT Broker
- Dùng broker public: `broker.hivemq.com`
- Không cần authentication
- QoS = 0 (fire and forget)
- Không có retained messages

### 3. Data Rate
- Telemetry: ~3 messages/giây (2 channels + combined)
- Status: ~2 messages/5 giây
- Heartbeat: 1 message/60 giây
- **Total**: ~180 messages/phút

### 4. Database Size
- 1 ngày = ~259,200 telemetry records
- 1 tuần = ~1.8 triệu records
- **Lưu ý**: Cần auto-delete data cũ hoặc aggregation

### 5. Real-time Requirement
- Telemetry update <2 giây delay
- Control command response <1 giây
- WebSocket preferred over polling

---

## 🎓 HỌC LIỆU THAM KHẢO

### MQTT
- https://mqtt.org/
- https://www.hivemq.com/mqtt-essentials/

### Node.js MQTT
- https://github.com/mqttjs/MQTT.js
- https://www.npmjs.com/package/mqtt

### Socket.IO
- https://socket.io/docs/v4/

### Chart.js
- https://www.chartjs.org/docs/latest/

### Express.js
- https://expressjs.com/

---

## 📞 LIÊN HỆ & HỖ TRỢ

### Hardware Team Contact
- **Người phụ trách**: [Tên của bạn]
- **Email**: [Email của bạn]
- **Telegram/Zalo**: [Số điện thoại]

### Support
- **ESP32 firmware**: Xem esp32_firmware/README.md
- **MQTT API**: Xem MQTT_API_DOCUMENTATION.md
- **Integration**: Xem INTEGRATION_GUIDE.md
- **Bugs/Issues**: Liên hệ trực tiếp

---

## ✅ XÁC NHẬN BÀN GIAO

### Hardware Team
- [ ] Đã test ESP32 hoạt động ổn định
- [ ] Đã cung cấp đầy đủ tài liệu
- [ ] Đã demo các Python test scripts
- [ ] Đã giải thích MQTT topics
- [ ] Sẵn sàng support khi cần

**Người bàn giao**: ________________  
**Ngày**: ___/___/2025

### Web/Database Team
- [ ] Đã nhận và đọc tài liệu
- [ ] Đã test MQTT monitor
- [ ] Đã hiểu rõ yêu cầu
- [ ] Đã cài đặt tools cần thiết
- [ ] Sẵn sàng bắt đầu coding

**Người nhận**: ________________  
**Ngày**: ___/___/2025

---

## 🎯 SUCCESS CRITERIA

Dự án hoàn thành khi:

1. ✅ **Database**: Data từ ESP32 được lưu liên tục
2. ✅ **Web Display**: Dashboard hiển thị V, I, P real-time
3. ✅ **Control**: Bật/tắt từ web → ESP32 phản ứng
4. ✅ **Simulator**: Slider điều chỉnh → Thấy current thay đổi
5. ✅ **Chart**: Hiển thị lịch sử ít nhất 1 giờ
6. ✅ **Demo**: Presentation mượt mà, không lỗi

**Deadline đề xuất**: 1 tuần từ ngày bàn giao

---

## 🚀 QUICK START

```bash
# 1. Test MQTT connection
cd esp32_firmware
python mqtt_monitor.py
# → Phải thấy data streaming

# 2. Test control
python test_control.py
# → Chọn 1 để bật CH1, xem đèn sáng

# 3. Đọc tài liệu
code ../MQTT_API_DOCUMENTATION.md
code ../INTEGRATION_GUIDE.md

# 4. Bắt đầu code
mkdir ../iot_backend
cd ../iot_backend
npm init -y
npm install express mqtt mysql2 socket.io cors

# 5. Code theo INTEGRATION_GUIDE.md
```

---

**Chúc team Web/Database làm việc hiệu quả! 🎉**

Có vấn đề gì hãy liên hệ ngay, hardware team sẵn sàng support! 💪
