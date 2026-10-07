cmake_minimum_required(VERSION 3.10)
project(DeviceControlSystem VERSION 1.0 LANGUAGES CXX)

# Yêu cầu C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Chỉ khai báo duy nhất một hàm add_executable ở đây
add_executable(device_control
    src/main.cpp
    src/Device.cpp
)

# Cấu hình thư mục include chứa file header (.h)
target_include_directories(device_control
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
