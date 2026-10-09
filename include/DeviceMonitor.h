#pragma once
#include <thread>
#include "Device.h"

class DeviceMonitor
{
public:
    // Constructor nhận vào tham chiếu đến đối tượng Device gốc
    DeviceMonitor(Device &device);

    // Destructor tự động giải phóng luồng an toàn theo chuẩn RAII
    ~DeviceMonitor();

    // Kích hoạt luồng ngầm chạy song song
    void start();

    // Ra lệnh dừng luồng ngầm và thực hiện ép luồng chính đứng đợi (join)
    void stop();

private:
    // Giữ tham chiếu hằng/biến tới thiết bị gốc để chia sẻ trạng thái giữa các luồng
    Device &device;

    // Đối tượng quản lý luồng ngầm của C++
    std::thread monitorThread;

    // Biến cờ hiệu quản lý vòng lặp chạy ngầm của luồng
    bool running;

    // Hàm chứa vòng lặp thực thi chính chạy bên trong luồng ngầm
    void monitorLoop();
};
