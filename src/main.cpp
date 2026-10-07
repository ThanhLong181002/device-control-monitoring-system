#include <iostream>
#include "Device.h"

// Hàm phụ trợ đổi enum sang chữ để in ra màn hình đẹp hơn
std::string stateToString(DeviceState state)
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

// Hàm phụ trợ in nhanh trạng thái hiện tại của thiết bị
void printDeviceStatus(const Device &device)
{
    std::cout << "State: " << stateToString(device.getState()) << "\n";
    std::cout << "Temperature: " << device.getTemperature() << " C\n";
    std::cout << "Motor: " << (device.isMotorRunning() ? "ON" : "OFF") << "\n";
    std::cout << "Conveyor: " << (device.isConveyorRunning() ? "ON" : "OFF") << "\n\n";
}

int main()
{
    std::cout << "Device Control & Monitoring System\n\n";

    // Khởi tạo thiết bị
    Device myDevice;
    std::cout << "Initial state: " << stateToString(myDevice.getState()) << "\n\n";

    // 1. Thử Nghiệm START
    std::cout << "Starting device...\n";
    myDevice.start();
    printDeviceStatus(myDevice);

    // 2. Thử Nghiệm STOP
    std::cout << "Stopping device...\n";
    myDevice.stop();
    printDeviceStatus(myDevice);

    // 3. Thử Nghiệm EMERGENCY STOP
    std::cout << "Emergency stop...\n";
    myDevice.emergencyStop();
    printDeviceStatus(myDevice);

    // 4. Thử Nghiệm RESET
    std::cout << "Resetting device...\n";
    myDevice.reset();
    printDeviceStatus(myDevice);

    return 0;
}
