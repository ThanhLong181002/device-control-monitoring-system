#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // Chặn không cho windows.h kéo theo winsock.h cũ vào hệ thống
#endif

#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>
#include <windows.h> // Đưa windows.h xuống dưới sau khi đã dọn dẹp môi trường macro

#include "TcpServer.h"
#include "ControlApplication.h"

// Cờ nguyên tử toàn cục báo hiệu yêu cầu shutdown hệ thống từ Control Handler
std::atomic<bool> shutdownRequested{false};

// Hàm Handler xử lý tín hiệu ngắt điều khiển Console của Windows
BOOL WINAPI consoleHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT ||
        signal == CTRL_BREAK_EVENT ||
        signal == CTRL_CLOSE_EVENT)
    {
        shutdownRequested.store(true);
        return TRUE;
    }
    return FALSE;
}

int main(int argc, char *argv[])
{
    // Đăng ký bộ xử lý tín hiệu với hệ điều hành Windows ngay khi khởi động
    if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
    {
        std::cerr << "[ERROR] Failed to register console control handler.\n";
        return 1;
    }

    if (argc < 2)
    {
        std::cout << "=== Device Control & Monitoring System ===\n";
        std::cout << "Usage error! Please choose a mode to run:\n";
        std::cout << "  To run Server:  .\\build\\Debug\\device_control.exe server\n";
        std::cout << "  To run Client:  .\\build\\Debug\\device_control.exe client\n";
        return 1;
    }

    // ĐÃ SỬA: Lấy chính xác chuỗi tham số từ vị trí số 1 (server hoặc client)
    std::string mode = argv[1];

    if (mode == "server")
    {
        std::cout << "=========================================\n";
        std::cout << "     MODE: TCP SERVER REAL-TIME CORE     \n";
        std::cout << "=========================================\n";

        TcpServer server(5000);

        if (server.start())
        {
            // Tách server.run() chạy độc lập ở luồng phụ tách biệt với main thread
            std::thread serverThread(&TcpServer::run, &server);

            std::cout << "[INFO] Press Ctrl+C to stop the server.\n";

            // Luồng chính ngủ đông, thức giấc mỗi 100ms quét cờ bẫy tín hiệu hệ thống
            while (!shutdownRequested.load())
            {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(100));
            }

            std::cout << "\n[INFO] Shutdown requested.\n";

            // Tiến hành chu trình tắt máy tuần tự
            server.requestStop();
            serverThread.join();
            server.stop();
        }
    }
    else if (mode == "client")
    {
        ControlApplication app("127.0.0.1", 5000);
        app.run();
    }
    else
    {
        std::cerr << "Invalid mode! Please type 'server' or 'client'.\n";
        return 1;
    }

    return 0;
}
