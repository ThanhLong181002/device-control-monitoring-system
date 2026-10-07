#include "Device.h"

// 1. Khởi tạo trạng thái ban đầu trong Constructor
Device::Device()
    : state(DeviceState::IDLE),
      temperature(25.0),
      motorRunning(false),
      conveyorRunning(false),
      errorMessage("")
{
}

// 2. Logic khi bấm nút START
void Device::start()
{
    // Chỉ cho phép chuyển sang RUNNING nếu đang ở IDLE hoặc chính nó là RUNNING
    if (state == DeviceState::IDLE || state == DeviceState::RUNNING)
    {
        state = DeviceState::RUNNING;
        motorRunning = true;
        conveyorRunning = true;
    }
    // Nếu đang ở trạng thái ERROR hoặc EMERGENCY_STOP thì bấm START sẽ không có tác dụng!
}

// 3. Logic khi bấm nút STOP
void Device::stop()
{
    if (state == DeviceState::RUNNING || state == DeviceState::IDLE)
    {
        state = DeviceState::IDLE;
        motorRunning = false;
        conveyorRunning = false;
    }
}

// 4. Bất kỳ trạng thái nào cũng có thể nhấn EMERGENCY STOP
void Device::emergencyStop()
{
    state = DeviceState::EMERGENCY_STOP;
    motorRunning = false;
    conveyorRunning = false;
}

// 5. Hàm RESET để sửa lỗi
void Device::reset()
{
    if (state == DeviceState::ERROR || state == DeviceState::EMERGENCY_STOP)
    {
        state = DeviceState::IDLE;
        motorRunning = false;
        conveyorRunning = false;
        errorMessage = "";
    }
}

// 6. Triển khai các hàm Getters (Đọc dữ liệu)
DeviceState Device::getState() const { return state; }
double Device::getTemperature() const { return temperature; }
bool Device::isMotorRunning() const { return motorRunning; }
bool Device::isConveyorRunning() const { return conveyorRunning; }
