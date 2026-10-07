#pragma once
#include <string>
#include "DeviceState.h" // Để sử dụng enum DeviceState

class Device
{
public:
    // Constructor (Hàm khởi tạo): Thiết lập giá trị ban đầu khi thiết bị vừa được tạo ra
    Device();

    // Các hàm hành vi (Commands)
    void start();
    void stop();
    void reset();
    void emergencyStop();

    // Các hàm lấy thông tin (Getters) - Thêm const vì không sửa đổi dữ liệu của class
    DeviceState getState() const;
    double getTemperature() const;
    bool isMotorRunning() const;    // Hàm bổ sung để main.cpp kiểm tra trạng thái motor
    bool isConveyorRunning() const; // Hàm bổ sung để main.cpp kiểm tra trạng thái băng tải

private:
    // Thuộc tính (Data members) - Giấu kín trong private
    DeviceState state;
    double temperature;
    bool motorRunning;
    bool conveyorRunning;
    std::string errorMessage;
};
