#pragma once
#include <string>
#include "TcpClient.h"

class ControlApplication
{
public:
    ControlApplication(const std::string &serverIp, int serverPort);
    ~ControlApplication() = default;

    void run();

private:
    TcpClient client;

    // ĐỀ XUẤT CỦA BẠN: Khởi tạo trực tiếp tại declaration cực kỳ an toàn
    bool keepRunning{true};

    void showMenu() const;
    void handleChoice(int choice);
    void sendCommandAndPrint(const std::string &commandName, const std::string &protocolPayload);
};
