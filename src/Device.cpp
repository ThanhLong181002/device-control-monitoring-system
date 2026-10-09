#include "Device.h"

// 1. Constructor sử dụng Member Initializer List (Giữ nguyên như cũ)
Device::Device()
    : state(DeviceState::IDLE),
      temperature(25.0),
      motorRunning(false),
      conveyorRunning(false),
      errorMessage("")
{
}

// 2. getStatus() const - Trả về bản sao snapshot giá trị dưới sự bảo vệ của mutex hằng (mutable)
DeviceStatus Device::getStatus() const
{
    // Khóa duy nhất một lần cho toàn bộ quá trình chụp ảnh dữ liệu
    std::lock_guard<std::mutex> lock(deviceMutex);

    // Trả về một bản sao thô đóng đóng băng dữ liệu tại thời điểm khóa, an toàn luồng tuyệt đối
    return {state, temperature, motorRunning, conveyorRunning, errorMessage};
}

// 3. Logic update() được bọc khóa bảo vệ cảm biến nhiệt độ chạy ngầm
void Device::update()
{
    std::lock_guard<std::mutex> lock(deviceMutex); // Tự động khóa

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
} // Tự động mở khóa khi thoát phạm vi hàm

// 4. Logic start() bọc khóa bảo vệ dữ liệu lệnh vận hành
void Device::start()
{
    std::lock_guard<std::mutex> lock(deviceMutex);

    if (state == DeviceState::IDLE || state == DeviceState::RUNNING)
    {
        state = DeviceState::RUNNING;
        motorRunning = true;
        conveyorRunning = true;
    }
}

// 5. Logic stop() bọc khóa bảo vệ lệnh dừng thiết bị
void Device::stop()
{
    std::lock_guard<std::mutex> lock(deviceMutex);

    if (state == DeviceState::RUNNING || state == DeviceState::IDLE)
    {
        state = DeviceState::IDLE;
        motorRunning = false;
        conveyorRunning = false;
    }
}

// 6. Logic emergencyStop() bọc khóa bảo vệ nút dừng khẩn cấp
void Device::emergencyStop()
{
    std::lock_guard<std::mutex> lock(deviceMutex);

    state = DeviceState::EMERGENCY_STOP;
    motorRunning = false;
    conveyorRunning = false;
}

// 7. Logic simulateError() bọc khóa bảo vệ cơ chế giả lập lỗi sự cố
void Device::simulateError()
{
    std::lock_guard<std::mutex> lock(deviceMutex);

    state = DeviceState::ERROR;
    motorRunning = false;
    conveyorRunning = false;
    errorMessage = "Simulated device fault";
}

// 8. Logic reset() bọc khóa bảo vệ cơ chế khôi phục dọn dẹp lỗi sự cố
void Device::reset()
{
    std::lock_guard<std::mutex> lock(deviceMutex);

    if (state == DeviceState::ERROR || state == DeviceState::EMERGENCY_STOP)
    {
        state = DeviceState::IDLE;
        motorRunning = false;
        conveyorRunning = false;
        errorMessage = "";
    }
}
