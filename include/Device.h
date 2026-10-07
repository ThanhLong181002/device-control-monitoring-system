#pragma once
#include <string>
#include "DeviceState.h"

class Device
{
public:
    Device();

    void update();
    void start();
    void stop();
    void reset();
    void emergencyStop();

    // Hàm mới cho Phase 1.7
    void simulateError();
    const std::string &getErrorMessage() const; // Trả về tham chiếu hằng để tối ưu hiệu năng

    DeviceState getState() const;
    double getTemperature() const;
    bool isMotorRunning() const;
    bool isConveyorRunning() const;

private:
    DeviceState state;
    double temperature;
    bool motorRunning;
    bool conveyorRunning;
    std::string errorMessage;

    static constexpr double MIN_TEMP = 25.0;
    static constexpr double MAX_TEMP = 80.0;
    static constexpr double TEMP_INC = 0.2;
    static constexpr double TEMP_DEC = 0.1;
};
