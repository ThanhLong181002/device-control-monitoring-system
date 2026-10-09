#include "DeviceMonitor.h"
#include <iostream>
#include <chrono> // Thư viện cấu hình thời gian (milliseconds, seconds)

// 1. Constructor: Sử dụng Member Initializer List để lưu tham chiếu và đặt cờ mặc định
DeviceMonitor::DeviceMonitor(Device &device)
    : device(device),
      running(false) // Đảm bảo luồng chưa chạy khi vừa khởi tạo đối tượng
{
}

// 2. Destructor: Đảm bảo giải phóng luồng tự động theo chuẩn RAII khi đối tượng bị hủy
DeviceMonitor::~DeviceMonitor()
{
    // Chỉ cần gọi hàm stop(), cơ chế kiểm tra bên trong stop() sẽ lo liệu việc join an toàn
    stop();
}

// 3. start(): Kích hoạt luồng ngầm (Có phòng vệ chống tạo luồng trùng lặp)
void DeviceMonitor::start()
{
    // Phòng vệ: Nếu hệ thống đang chạy HOẶC thread cũ vẫn đang kẹt (joinable), không tạo thread mới
    if (running || monitorThread.joinable())
    {
        std::cout << "[WARNING] DeviceMonitor: Monitor thread is already running or active.\n";
        return;
    }

    // Kích hoạt cờ hiệu trước khi bật luồng
    running = true;

    // Khởi tạo std::thread, truyền con trỏ hàm thành viên &DeviceMonitor::monitorLoop
    // và con trỏ 'this' để luồng phụ biết đối tượng quản lý logic
    monitorThread = std::thread(&DeviceMonitor::monitorLoop, this);
    std::cout << "[INFO] DeviceMonitor: Real-time monitoring thread started successfully.\n";
}

// 4. stop(): Dừng luồng ngầm từ xa một cách an toàn
void DeviceMonitor::stop()
{
    // Nếu cờ hiệu đang là false và thread không dùng được nữa, bỏ qua để tránh gọi trùng lặp
    if (!running && !monitorThread.joinable())
    {
        return;
    }

    // Bước A: Hạ cờ running để bẻ gãy vòng lặp while trong monitorLoop ngầm
    running = false;

    // Bước B: Kiểm tra điều kiện an toàn, nếu thread đang quản lý một luồng sống thì mới ép luồng chính đứng đợi
    if (monitorThread.joinable())
    {
        monitorThread.join(); // Treo luồng chính lại cho đến khi monitorLoop kết thúc hoàn toàn
        std::cout << "[INFO] DeviceMonitor: Monitoring thread has been joined and stopped cleanly.\n";
    }
}

// 5. monitorLoop(): Vòng lặp chạy ngầm xuyên suốt chu kỳ sống của luồng phụ
void DeviceMonitor::monitorLoop()
{
    // Vòng lặp chạy liên tục miễn là cờ running chưa bị hạ về false
    while (running)
    {
        // Cập nhật trạng thái cảm biến (nhiệt độ) của thiết bị ảo
        device.update();

        // Đưa luồng ngầm vào trạng thái ngủ (Interval 100ms) để giải phóng CPU cho hệ thống
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
