import paho.mqtt.client as mqtt
import time

BROKER = "broker.hivemq.com"
PORT = 1883
DEVICE_ID = "anh_hong_dep_trai_ittn"

print("=" * 60)
print("TEST ĐIỀU KHIỂN ESP32 QUA MQTT")
print("=" * 60)

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("✓ Đã kết nối broker\n")
        
        # Bật CH1 (Đèn)
        topic = f"devices/{DEVICE_ID}/ch1/switch/set"
        print(f"📤 [1] BẬT CHANNEL 1 (ĐÈN)")
        print(f"    Topic: {topic}")
        print(f"    Value: ON\n")
        client.publish(topic, "ON", qos=1)
        time.sleep(3)
        
        # Bật CH2 (Quạt)
        topic2 = f"devices/{DEVICE_ID}/ch2/switch/set"
        print(f"📤 [2] BẬT CHANNEL 2 (QUẠT)")
        print(f"    Topic: {topic2}")
        print(f"    Value: ON\n")
        client.publish(topic2, "ON", qos=1)
        time.sleep(3)
        
        # Tắt CH1
        print(f"📤 [3] TẮT CHANNEL 1 (ĐÈN)")
        print(f"    Topic: {topic}")
        print(f"    Value: OFF\n")
        client.publish(topic, "OFF", qos=1)
        time.sleep(3)
        
        # Tắt CH2
        print(f"📤 [4] TẮT CHANNEL 2 (QUẠT)")
        print(f"    Topic: {topic2}")
        print(f"    Value: OFF\n")
        client.publish(topic2, "OFF", qos=1)
        time.sleep(2)
        
        print("=" * 60)
        print("✅ HOÀN THÀNH!")
        print("=" * 60)
        print("\nKiểm tra:")
        print("- Serial Monitor để thấy ESP32 nhận lệnh")
        print("- MQTT Monitor để thấy phản hồi status/telemetry")
        print("- Đèn và Quạt có bật/tắt theo lệnh không\n")
        
        client.disconnect()

client = mqtt.Client(client_id="control_test", callback_api_version=mqtt.CallbackAPIVersion.VERSION1)
client.on_connect = on_connect

try:
    client.connect(BROKER, PORT, 60)
    client.loop_forever()
except Exception as e:
    print(f"✗ Lỗi: {e}")
