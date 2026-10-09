#pragma once
#include <string>

// Cấu hình tiền xử lý để kéo thư viện Winsock2 trên Windows
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#endif

class TcpClient
{
public:
    // Constructor nhận vào IP và Cổng mạng của Server cần kết nối tới
    TcpClient(const std::string &ip, int port);

    // Destructor tự động dọn dẹp kết nối theo chuẩn RAII nếu người dùng quên gọi disconnect
    ~TcpClient();

    // Kết nối tới Server (Khởi tạo Winsock, tạo socket và gọi hàm connect)
    bool connectToServer();

    // Gửi một chuỗi tin nhắn sang Server
    bool sendMessage(const std::string &message);

    // Chờ nhận phản hồi từ Server và trả về chuỗi kết quả
    std::string receiveResponse();

    // Đóng socket kết nối và giải phóng tài nguyên Winsock của Client
    void disconnect();

private:
    std::string serverIp;
    int serverPort;

    // Quản lý Socket kết nối đặc trưng của Windows
    SOCKET connectSocket;

    // Biến cờ hiệu quản lý trạng thái kết nối hiện tại của Client
    bool isConnected;

    // Hàm phụ trợ khởi tạo thư viện mạng Windows
    bool initWinsock();
};
