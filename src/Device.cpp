#include "Device.h"

// 1. Constructor sử dụng Member Initializer List
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
    if (state == DeviceState::IDLE || state == DeviceState::RUNNING)
    {
        state = DeviceState::RUNNING;
        motorRunning = true;
        conveyorRunning = true;
    }
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

// 5. Hàm update() cập nhật trạng thái cảm biến nhiệt độ
void Device::update()
{
    switch (state)
    {
    case DeviceState::RUNNING:
        temperature += TEMP_INC;
        if (temperature > MAX_TEMP)
        {
            temperature = MAX_TEMP;
        }
        break;

    case DeviceState::IDLE:
        temperature -= TEMP_DEC;
        if (temperature < MIN_TEMP)
        {
            temperature = MIN_TEMP;
        }
        break;

    case DeviceState::EMERGENCY_STOP:
    case DeviceState::ERROR:
        // Giữ nguyên nhiệt độ
        break;
    }
}

// 6. Giả lập lỗi thiết bị
void Device::simulateError()
{
    state = DeviceState::ERROR;
    motorRunning = false;
    conveyorRunning = false;
    errorMessage = "Simulated device fault";
}

// 7. Hàm RESET duy nhất (Đã xóa hàm cũ ở trên đi)
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

// 8. Các hàm Getters đọc dữ liệu
const std::string &Device::getErrorMessage() const { return errorMessage; }
DeviceState Device::getState() const { return state; }
double Device::getTemperature() const { return temperature; }
bool Device::isMotorRunning() const { return motorRunning; }
bool Device::isConveyorRunning() const { return conveyorRunning; }
