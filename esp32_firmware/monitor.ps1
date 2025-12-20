# ESP32 Serial Monitor Script
# Đọc dữ liệu từ ESP32 qua COM port

$portName = "COM4"
$baudRate = 115200

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "ESP32 Serial Monitor" -ForegroundColor Cyan
Write-Host "Port: $portName | Baud: $baudRate" -ForegroundColor Cyan
Write-Host "Nhấn Ctrl+C để thoát" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

try {
    # Tạo SerialPort object
    $port = New-Object System.IO.Ports.SerialPort $portName, $baudRate
    $port.Open()
    
    Write-Host "✓ Đã kết nối tới $portName" -ForegroundColor Green
    Write-Host "✓ Đang đọc dữ liệu từ ESP32..." -ForegroundColor Green
    Write-Host ""
    Write-Host "--- LOG BẮT ĐẦU ---" -ForegroundColor Yellow
    Write-Host ""
    
    # Đọc liên tục
    while ($true) {
        if ($port.BytesToRead -gt 0) {
            $line = $port.ReadLine()
            Write-Host $line
        }
        Start-Sleep -Milliseconds 10
    }
}
catch [System.UnauthorizedAccessException] {
    Write-Host ""
    Write-Host "❌ LỖI: Không thể mở cổng $portName" -ForegroundColor Red
    Write-Host ""
    Write-Host "NGUYÊN NHÂN: Cổng đang được sử dụng bởi chương trình khác" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "GIẢI PHÁP:" -ForegroundColor Cyan
    Write-Host "1. Đóng VS Code hoàn toàn" -ForegroundColor White
    Write-Host "2. Rút dây USB ESP32, đợi 3 giây, cắm lại" -ForegroundColor White
    Write-Host "3. Chạy lại script này:" -ForegroundColor White
    Write-Host "   powershell -ExecutionPolicy Bypass -File monitor.ps1" -ForegroundColor Gray
    Write-Host ""
}
catch {
    Write-Host ""
    Write-Host "❌ LỖI: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host ""
}
finally {
    if ($port -ne $null -and $port.IsOpen) {
        $port.Close()
        Write-Host ""
        Write-Host "✓ Đã đóng cổng Serial" -ForegroundColor Green
    }
}
