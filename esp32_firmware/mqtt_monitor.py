import paho.mqtt.client as mqtt
import time

BROKER = "broker.hivemq.com"
PORT = 1883
DEVICE_ID = "anh_hong_dep_trai_ittn"
BASE_TOPIC = f"devices/{DEVICE_ID}"

print("=" * 60)
print("ESP32 MQTT MONITOR")
print("=" * 60)
print(f"Broker: {BROKER}:{PORT}")
print(f"Device: {DEVICE_ID}")
print(f"Topic: {BASE_TOPIC}/#")
print("=" * 60)
print("\nĐang kết nối tới MQTT broker...")

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("✓ Đã kết nối tới MQTT broker!")
        print(f"✓ Đang subscribe topic: {BASE_TOPIC}/#\n")
        client.subscribe(f"{BASE_TOPIC}/#")
        print("--- ĐANG CHỜ DỮ LIỆU TỪ ESP32 ---\n")
    else:
        print(f"✗ Kết nối thất bại với code {rc}")

def on_message(client, userdata, msg):
    print(f"📩 [{time.strftime('%H:%M:%S')}] {msg.topic}")
    print(f"   {msg.payload.decode()}\n")

def on_disconnect(client, userdata, rc):
    print(f"\n⚠ Ngắt kết nối MQTT (code: {rc})")

# Tạo MQTT client
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.on_disconnect = on_disconnect

try:
    client.connect(BROKER, PORT, 60)
    client.loop_forever()
except KeyboardInterrupt:
    print("\n\n✓ Đã dừng monitor")
except Exception as e:
    print(f"\n✗ Lỗi: {e}")
finally:
    client.disconnect()
