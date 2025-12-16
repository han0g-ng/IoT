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

## 📞 Liên hệ

Nếu gặp vấn đề, hãy mở issue hoặc liên hệ qua email.
