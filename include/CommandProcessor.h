#pragma once
#include <string>
#include "Device.h"

// Hàm xử lý lệnh nhận vào tham chiếu đến đối tượng Device thực tế
// và chuỗi lệnh thô từ Client (bỏ đi ký tự xuống dòng nếu có)
std::string processCommand(Device &device, const std::string &command);
