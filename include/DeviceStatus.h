#pragma once
#include <string>
#include "DeviceState.h"

// Cấu trúc snapshot chứa toàn bộ trạng thái nhất quán tại một thời điểm
struct DeviceStatus
{
    DeviceState state;
    double temperature;
    bool motorRunning;
    bool conveyorRunning;
    std::string errorMessage;
};
