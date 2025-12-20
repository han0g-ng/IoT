import paho.mqtt.client as mqtt
import time

BROKER = "broker.hivemq.com"
PORT = 1883
DEVICE_ID = "anh_hong_dep_trai_ittn"

print("=" * 60)
print("TEST MQTT - GỬI LỆNH TỚI ESP32")
print("=" * 60)

client = mqtt.Client(client_id="test_publisher")

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("✓ Đã kết nối broker")
        
        # Gửi lệnh bật channel 1
        topic = f"devices/{DEVICE_ID}/ch1/switch/set"
        print(f"\n📤 Gửi lệnh: {topic} = ON")
        client.publish(topic, "ON")
        
        time.sleep(2)
        
        # Gửi lệnh bật channel 2
        topic2 = f"devices/{DEVICE_ID}/ch2/switch/set"
        print(f"📤 Gửi lệnh: {topic2} = ON")
        client.publish(topic2, "ON")
        
        time.sleep(2)
        
        print("\n✓ Đã gửi test commands")
        print("\nNếu ESP32 nhận được, bạn sẽ thấy phản hồi trong mqtt_monitor.py")
        print("Hoặc kiểm tra LED trên ESP32 có thay đổi không\n")
        
        client.disconnect()

client.on_connect = on_connect

try:
    client.connect(BROKER, PORT, 60)
    client.loop_start()
    time.sleep(5)
    client.loop_stop()
except Exception as e:
    print(f"✗ Lỗi: {e}")
