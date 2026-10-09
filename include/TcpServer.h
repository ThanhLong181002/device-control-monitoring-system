#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#endif

#include <string>
#include <atomic>
#include "Device.h"
#include "DeviceMonitor.h"

class TcpServer
{
public:
    TcpServer(int port);
    ~TcpServer();

    bool start();
    void run();
    void stop();
    void requestStop();

private:
    int port;
    SOCKET listenSocket;
    SOCKET clientSocket;

    std::atomic<bool> isRunning{false};

    // THÊM DÒNG NÀY: Quản lý độc lập trạng thái khởi tạo Winsock
    bool winsockInitialized{false};

    Device device;
    DeviceMonitor monitor;

    bool initWinsock();
};
