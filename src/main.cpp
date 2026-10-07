#include <iostream>
#include "Device.h"

// Hàm phụ trợ đổi enum sang chữ để in ra màn hình
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

// Hàm phụ trợ in nhanh trạng thái thiết bị
void printDeviceStatus(const Device &device)
{
    std::cout << "State: " << stateToString(device.getState()) << "\n";
    std::cout << "Motor: " << (device.isMotorRunning() ? "ON" : "OFF") << "\n";
    std::cout << "Conveyor: " << (device.isConveyorRunning() ? "ON" : "OFF") << "\n";
    if (!device.getErrorMessage().empty())
    {
        std::cout << "Error Message: " << device.getErrorMessage() << "\n";
    }
    std::cout << "\n";
}

int main()
{
    std::cout << "=== Device Control & Monitoring System ===\n";
    std::cout << "--- TESTING INVALID STATE TRANSITIONS ---\n\n";

    Device myDevice;

    // =================================================================
    // CHUỖI TEST 1: KIỂM TRA CHẶN LỆNH START KHI ĐANG Ở TRẠNG THÁI ERROR
    // =================================================================
    std::cout << ">>> SEQUENCE 1: RUNNING -> ERROR -> TRY START -> RESET -> START\n";

    std::cout << "1. Sending START command...\n";
    myDevice.start();
    printDeviceStatus(myDevice); // Expected: RUNNING, Motor: ON, Conveyor: ON

    std::cout << "2. Triggering Simulate Error...\n";
    myDevice.simulateError();
    printDeviceStatus(myDevice); // Expected: ERROR, Motor: OFF, Conveyor: OFF

    std::cout << "3. Trying to START while in ERROR state (Should be blocked!)...\n";
    myDevice.start();
    printDeviceStatus(myDevice); // Expected: VẪN ERROR, Motor: OFF, Conveyor: OFF

    std::cout << "4. Sending RESET command...\n";
    myDevice.reset();
    printDeviceStatus(myDevice); // Expected: IDLE, Motor: OFF, Conveyor: OFF

    std::cout << "5. Sending START command again (Should work now)...\n";
    myDevice.start();
    printDeviceStatus(myDevice); // Expected: RUNNING, Motor: ON, Conveyor: ON

    // =================================================================
    // CHUỖI TEST 2: KIỂM TRA CHẶN LỆNH START KHI ĐANG EMERGENCY_STOP
    // =================================================================
    std::cout << ">>> SEQUENCE 2: EMERGENCY STOP -> TRY START -> RESET -> START\n";

    std::cout << "1. Triggering EMERGENCY STOP...\n";
    myDevice.emergencyStop();
    printDeviceStatus(myDevice); // Expected: EMERGENCY_STOP, Motor: OFF, Conveyor: OFF

    std::cout << "2. Trying to START while in EMERGENCY_STOP state (Should be blocked!)...\n";
    myDevice.start();
    printDeviceStatus(myDevice); // Expected: VẪN EMERGENCY_STOP, Motor: OFF, Conveyor: OFF

    std::cout << "3. Sending RESET command...\n";
    myDevice.reset();
    printDeviceStatus(myDevice); // Expected: IDLE, Motor: OFF, Conveyor: OFF

    std::cout << "4. Sending START command again (Should work now)...\n";
    myDevice.start();
    printDeviceStatus(myDevice); // Expected: RUNNING, Motor: ON, Conveyor: ON

    return 0;
}
