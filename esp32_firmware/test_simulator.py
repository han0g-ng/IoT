import paho.mqtt.client as mqtt
import time

BROKER = "broker.hivemq.com"
DEVICE_ID = "anh_hong_dep_trai_ittn"

print("="*60)
print("TEST SIMULATOR - MÔ PHỎNG LỖI NGUỒN")
print("="*60)

def send_command(client, topic, value, desc):
    print(f"\n📤 {desc}")
    print(f"   Topic: {topic}")
    print(f"   Value: {value}")
    client.publish(topic, value, qos=1)
    time.sleep(2)

client = mqtt.Client(client_id="sim_test")
client.connect(BROKER, 1883, 60)
client.loop_start()
time.sleep(1)

# Test 1: Bật đèn CH1
send_command(client, f"devices/{DEVICE_ID}/ch1/switch/set", "ON", 
             "[1] BẬT ĐÈN CH1 - HOẠT ĐỘNG BÌNH THƯỜNG")

# Test 2: Simulator 0% - Không có lỗi
send_command(client, f"devices/{DEVICE_ID}/ch1/sim/set", "0",
             "[2] SIMULATOR 0% - Hoạt động bình thường")

# Test 3: Simulator 30% - Giảm công suất 30%
send_command(client, f"devices/{DEVICE_ID}/ch1/sim/set", "30",
             "[3] SIMULATOR 30% - Giảm công suất 30%")

# Test 4: Simulator 60% - Giảm công suất 60%
send_command(client, f"devices/{DEVICE_ID}/ch1/sim/set", "60",
             "[4] SIMULATOR 60% - Giảm công suất 60%")

# Test 5: Simulator 100% - Ngắt hoàn toàn (mô phỏng mở mạch)
send_command(client, f"devices/{DEVICE_ID}/ch1/sim/set", "100",
             "[5] SIMULATOR 100% - Mở mạch hoàn toàn")

# Test 6: Trở về bình thường
send_command(client, f"devices/{DEVICE_ID}/ch1/sim/set", "0",
             "[6] SIMULATOR 0% - Trở về bình thường")

# Test 7: Tắt đèn
send_command(client, f"devices/{DEVICE_ID}/ch1/switch/set", "OFF",
             "[7] TẮT ĐÈN CH1")

print("\n" + "="*60)
print("✅ HOÀN TẤT TEST SIMULATOR")
print("="*60)
print("\nKiểm tra MQTT Monitor để thấy:")
print("- Voltage/Current/Power thay đổi theo simulator")
print("- Khi simulator tăng → Current/Power giảm")
print("- Simulator 100% → Current gần 0 (mô phỏng mở mạch)\n")

client.loop_stop()
client.disconnect()
