cmake_minimum_required(VERSION 3.10)
project(DeviceControlSystem VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Đảm bảo điền đầy đủ cả 7 file mã nguồn của dự án hiện tại
add_executable(device_control
    src/main.cpp
    src/Device.cpp
    src/CommandProcessor.cpp
    src/TcpServer.cpp
    src/TcpClient.cpp
    src/ControlApplication.cpp
    src/DeviceMonitor.cpp  # FILE MỚI BỔ SUNG
)

target_include_directories(device_control
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

if(WIN32)
    target_link_libraries(device_control PRIVATE ws2_32)
endif()

include(CTest)

if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    )

    # Tránh xung đột cấu hình CRT trên Windows.
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)

    add_executable(device_tests
        tests/DeviceTests.cpp
        src/Device.cpp
    )

    target_include_directories(device_tests
        PRIVATE
            ${PROJECT_SOURCE_DIR}/include
    )

    target_link_libraries(device_tests
        PRIVATE
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(device_tests)
endif()