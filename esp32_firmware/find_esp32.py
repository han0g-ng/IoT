import serial.tools.list_ports
import time

print("=" * 60)
print("DANH SÁCH CỔNG COM")
print("=" * 60)

ports = serial.tools.list_ports.comports()
for port in ports:
    print(f"\n{port.device}")
    print(f"  Mô tả: {port.description}")
    print(f"  Hardware ID: {port.hwid}")
    if "CH340" in port.description or "CP210" in port.description:
        print(f"  ⭐ ĐÂY LÀ ESP32!")
        esp32_port = port.device
        
print("\n" + "=" * 60)

# Thử đọc từ ESP32
if 'esp32_port' in locals():
    print(f"\nThử mở {esp32_port}...")
    try:
        ser = serial.Serial(esp32_port, 115200, timeout=2)
        print(f"✓ Đã mở {esp32_port}!")
        print("✓ Đọc log trong 10 giây...\n")
        print("--- LOG ---")
        
        start_time = time.time()
        while time.time() - start_time < 10:
            if ser.in_waiting > 0:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print(line)
                    
        ser.close()
        print("\n✓ Đã đọc xong")
        
    except serial.SerialException as e:
        print(f"✗ Không thể mở: {e}")
        print("\n📋 HƯỚNG DẪN:")
        print("1. Đóng VS Code")
        print("2. Mở Task Manager (Ctrl+Shift+Esc)")
        print("3. Tìm 'Code.exe' → End Task tất cả")
        print("4. Rút dây USB ESP32, đợi 5 giây, cắm lại")
        print("5. Chạy lại script này")
else:
    print("\n⚠ Không tìm thấy ESP32!")
