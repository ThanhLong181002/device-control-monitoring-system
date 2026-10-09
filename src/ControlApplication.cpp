#include "ControlApplication.h"
#include <conio.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <cstdlib>
#include <string>

// 1. Constructor sử dụng Member Initializer List để khởi tạo đối tượng client nội bộ
ControlApplication::ControlApplication(const std::string &serverIp, int serverPort)
    : client(serverIp, serverPort)
{
    // Biến keepRunning đã được khởi tạo mặc định là true ở file .h nhờ brace initialization
}

// 2. showMenu() - Đóng vai trò hiển thị giao diện UI (Hàm const an toàn)
void ControlApplication::showMenu() const
{
    std::cout << "\n";
    std::cout << "==================================================\n";
    std::cout << "          DEVICE CONTROL & MONITORING\n";
    std::cout << "==================================================\n";
    std::cout << "  Industrial Device Simulator | C++17 | TCP/IP\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "  DEVICE COMMANDS\n";
    std::cout << "    [1]  START\n";
    std::cout << "    [2]  STOP\n";
    std::cout << "    [3]  RESET\n";
    std::cout << "    [4]  EMERGENCY STOP\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "  MONITORING\n";
    std::cout << "    [5]  Live Device Status\n";
    std::cout << "    [6]  Exit Application\n";
    std::cout << "==================================================\n";
    std::cout << "  Select command (1-6): ";
}

// 3. sendCommandAndPrint() - Helper xử lý đồng bộ luồng truyền nhận tin qua mạng
void ControlApplication::sendCommandAndPrint(const std::string &commandName, const std::string &protocolPayload)
{
    std::cout << "\n[CLIENT] Sending command: " << commandName << "...\n";
    if (client.sendMessage(protocolPayload))
    {
        std::string response = client.receiveResponse();
        std::cout << "[SERVER RESPONSE]: " << response;
    }
    else
    {
        std::cerr << "[ERROR] Failed to send command to server.\n";
    }
}

// 4. handleChoice() - Ánh xạ phím bấm từ giao diện sang hành động giao thức mạng tương ứng
void ControlApplication::handleChoice(int choice)
{
    switch (choice)
    {
    case 1:
        sendCommandAndPrint("START", "START\n");
        break;
    case 2:
        sendCommandAndPrint("STOP", "STOP\n");
        break;
    case 3:
        sendCommandAndPrint("RESET", "RESET\n");
        break;
    case 4:
        sendCommandAndPrint("EMERGENCY STOP", "EMERGENCY_STOP\n");
        break;
    case 5:
    { // SỬA LẠI RIÊNG CASE 5 THÀNH VÒNG LẶP LIVE MONITORING
        std::cout << "\n=========================================\n";
        std::cout << "     LIVE MONITORING MODE (Press 'q' to Exit)\n";
        std::cout << "=========================================\n";

        // Chạy vòng lặp vô hạn cho đến khi người dùng nhấn phím 'q'
        while (true)
        {
            // Kiểm tra xem người dùng có bấm phím nào trên bàn phím không
            if (_kbhit())
            {
                char ch = _getch();
                if (ch == 'q' || ch == 'Q')
                {
                    std::cout << "\n[INFO] Exiting Live Monitoring...\n";
                    break; // Thoát vòng lặp live, quay lại Menu chính
                }
            }

            // Tự động bắn lệnh STATUS sang Server
            if (client.sendMessage("STATUS\n"))
            {

                std::string response = client.receiveResponse();

                // Loại bỏ ký tự xuống dòng từ phản hồi của Server
                while (!response.empty() &&
                       (response.back() == '\n' || response.back() == '\r'))
                {
                    response.pop_back();
                }

                // In đè dữ liệu trên cùng một dòng
                std::cout << "\r[LIVE DATA]: " << response
                          << "                                      "
                          << std::flush;
            }

            // Nghỉ 500 mili-giây (0.5 giây) rồi tự động quét lại chu kỳ sau
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        break;
    }
    case 6:
        std::cout << "\n[CLIENT] Exiting application. Disconnecting from server...\n";
        keepRunning = false; // Bẻ gãy vòng lặp chạy của Menu
        break;
    default:
        std::cout << "\n[WARNING] Invalid choice! Please select a number between 1 and 6.\n\n";
        break;
    }
}

// 5. run() - Trái tim điều phối vòng đời ứng dụng Client
void ControlApplication::run()
{
    std::cout << "=========================================\n";
    std::cout << "     LAUNCHING: CONTROL APPLICATION      \n";
    std::cout << "=========================================\n";

    std::cout << "[CLIENT] Attempting to connect to Server...\n";
    // Tiến hành kết nối hạ tầng mạng thông qua đối tượng client đóng gói
    if (!client.connectToServer())
    {
        std::cerr << "[CLIENT] Connection failed! Please start the Server first.\n";
        return; // Thoát hàm ngay lập tức nếu không thông mạng
    }

    int choice = 0;

    // Vòng lặp quản lý chu kỳ sống của Menu tương tác
    while (keepRunning)
    {
        showMenu();

        // Xử lý kiểm tra phòng vệ dữ liệu nhập từ bàn phím (cin protection)
        if (!(std::cin >> choice))
        {
            std::cout << "\n[WARNING] Invalid input! Please enter a number.\n\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        // Clear màn hình trước khi xử lý lựa chọn
        std::system("cls");

        // Điều phối lựa chọn số
        handleChoice(choice);
        std::cout << "\n"; // Tạo dòng trống thoáng mắt cho chu kỳ Menu kế tiếp
    }
}
