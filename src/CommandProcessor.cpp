#include "CommandProcessor.h"
#include <sstream>
#include <iostream> // THÊM THƯ VIỆN NÀY ĐỂ SỬ DỤNG COUT / CERR

static std::string deviceStateToString(DeviceState state)
{
    switch (state)
    {
    case DeviceState::IDLE:
        return "IDLE";
    case DeviceState::RUNNING:
        return "RUNNING";
    case DeviceState::ERROR:
        return "ERROR";
    case DeviceState::EMERGENCY_STOP:
        return "EMERGENCY_STOP";
    default:
        return "UNKNOWN";
    }
}

std::string processCommand(Device &device, const std::string &command)
{
    // Bổ sung log đầu vào thô ngay khi tiếp nhận lệnh ở tầng bóc tách
    std::cout << "[COMMAND] Received: " << command << '\n';

    if (command == "START")
    {
        device.start();
        std::cout << "[INFO] Device START command processed.\n"; // Log kết quả xử lý
        return "OK\n";
    }
    else if (command == "STOP")
    {
        device.stop();
        std::cout << "[INFO] Device STOP command processed.\n"; // Log kết quả xử lý
        return "OK\n";
    }
    else if (command == "RESET")
    {
        device.reset();
        std::cout << "[INFO] Device RESET command processed.\n"; // Log kết quả xử lý
        return "OK\n";
    }
    else if (command == "EMERGENCY_STOP")
    {
        device.emergencyStop();
        std::cout << "[INFO] Device EMERGENCY_STOP command processed.\n"; // Log kết quả xử lý
        return "OK\n";
    }
    else if (command == "UPDATE")
    {
        device.update();
        std::cout << "[INFO] Device UPDATE command processed.\n"; // Log kết quả xử lý
        return "OK\n";
    }
    else if (command == "STATUS")
    {
        DeviceStatus status = device.getStatus();

        std::ostringstream oss;
        oss << "STATE=" << deviceStateToString(status.state) << ";"
            << "TEMP=" << status.temperature << ";"
            << "MOTOR=" << (status.motorRunning ? "ON" : "OFF") << ";"
            << "CONVEYOR=" << (status.conveyorRunning ? "ON" : "OFF") << ";"
            << "ERROR=" << status.errorMessage << "\n";

        std::cout << "[INFO] Device status requested.\n"; // Log kết quả xử lý trước khi return
        return oss.str();
    }
    else
    {
        // Ghi nhận log cảnh báo lỗi nghiêm trọng khi nhận lệnh sai cú pháp giao thức
        std::cerr << "[WARN] Unknown command: " << command << '\n';
        return "ERROR: UNKNOWN_COMMAND\n";
    }
}
