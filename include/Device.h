#pragma once
#include <string>
#include <mutex> // Nhúng thư viện mutex để bảo vệ đa luồng
#include "DeviceState.h"
#include "DeviceStatus.h" // Nhúng cấu trúc snapshot mới

class Device
{
public:
    Device();

    // Hàm cập nhật và các lệnh hành vi (Tất cả sẽ được bọc Lock bên trong file .cpp)
    void update();
    void start();
    void stop();
    void reset();
    void emergencyStop();
    void simulateError();

    // Hàm duy nhất thay thế toàn bộ getters cũ, trả về bản sao giá trị để an toàn luồng
    DeviceStatus getStatus() const;

private:
    DeviceState state;
    double temperature;
    bool motorRunning;
    bool conveyorRunning;
    std::string errorMessage;

    // Cơ chế đồng bộ hóa luồng ngầm độc lập của riêng lớp Device
    mutable std::mutex deviceMutex;

    static constexpr double MIN_TEMP = 25.0;
    static constexpr double MAX_TEMP = 80.0;
    static constexpr double TEMP_INC = 0.2;
    static constexpr double TEMP_DEC = 0.1;
};
