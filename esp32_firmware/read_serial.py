import serial
import time

print("Đang kết nối tới COM4...")
try:
    ser = serial.Serial('COM4', 115200, timeout=1)
    time.sleep(2)  # Đợi ESP32 khởi động
    
    print("=" * 60)
    print("Serial Monitor đang chạy - Nhấn Ctrl+C để thoát")
    print("=" * 60)
    
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if line:
                print(line)
                
except serial.SerialException as e:
    print(f"Lỗi: {e}")
    print("\nGiải pháp:")
    print("1. Đóng VS Code hoàn toàn")
    print("2. Rút dây USB ESP32 ra, cắm lại")
    print("3. Chạy lại script này")
except KeyboardInterrupt:
    print("\n\nĐã thoát Serial Monitor")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
