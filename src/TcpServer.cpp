#include "TcpServer.h"
#include "CommandProcessor.h"
#include <iostream>
#include <ws2tcpip.h>

// 1. Constructor bắt buộc phải có tiền tố TcpServer:: và nhận tham số int port
TcpServer::TcpServer(int port)
    : port(port),
      listenSocket(INVALID_SOCKET),
      clientSocket(INVALID_SOCKET),
      isRunning(false),
      device(),       // 1. Khởi tạo đối tượng thiết bị trước
      monitor(device) // 2. Khởi tạo bộ giám sát nhận tham chiếu thiết bị gốc ngay sau đó
{
    // Thân hàm bỏ trống sạch sẽ, không gán thêm monitor hay device ở trong này nữa
}

// Đảm bảo hàm hủy TcpServer::~TcpServer() được định nghĩa rõ ràng
TcpServer::~TcpServer()
{
    stop(); // Tự động dừng luồng monitor và đóng mạng an toàn khi server bị hủy
}

// 3. Hàm khởi tạo Winsock nội bộ
bool TcpServer::initWinsock()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "[ERROR] TcpServer: WSAStartup failed: " << result << "\n";
        return false;
    }

    // ĐẶT CỜ HIỆU TẠI ĐÂY: Hạ tầng mạng Windows đã kích hoạt thành công
    winsockInitialized = true;
    return true;
}

// 4. Hàm khởi động Server an toàn, trả về bool
bool TcpServer::start()
{
    if (!initWinsock())
        return false;

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        std::cerr << "[ERROR] TcpServer: Socket creation failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (bind(listenSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "[ERROR] TcpServer: Bind failed: " << WSAGetLastError() << "\n";
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "[ERROR] TcpServer: Listen failed: " << WSAGetLastError() << "\n";
        closesocket(listenSocket);
        WSACleanup();
        return false;
    }

    isRunning = true;

    monitor.start(); // <-- DÒNG MỚI BỔ SUNG

    std::cout << "[SUCCESS] Server is listening on 127.0.0.1:" << port << ". Waiting for client...\n";
    return true;
}

// 5. Vòng lặp nhận và xử lý nhiều lệnh liên tiếp từ Client trên một kết nối
void TcpServer::run()
{
    if (!isRunning)
        return;

    // VÒNG LẶP NGOÀI: Tiếp nhận kết nối từ các Client mới nối tiếp nhau
    while (isRunning)
    {
        // --- 1. BẢO VỆ VÒNG LẶP NGOÀI: SELECT() CHO LISSENSOCKET ---
        fd_set listenReadSet;
        FD_ZERO(&listenReadSet);
        FD_SET(listenSocket, &listenReadSet);

        timeval listenTimeout{0, 500000}; // Timeout 500ms

        int listenSelectResult = select(0, &listenReadSet, nullptr, nullptr, &listenTimeout);

        // THÊM ĐOẠN NÀY: Kiểm tra yêu cầu shutdown hệ thống ngay lập tức khi select() nhả luồng
        if (!isRunning)
        {
            break; // Thoát hẳn vòng lặp ngoài để xuống chu trình dọn dẹp sạch sẽ
        }

        // Cập nhật lại nhánh xử lý timeout: Chỉ continue khi chắc chắn server vẫn đang chạy
        if (listenSelectResult == 0)
        {
            continue;
        }

        if (listenSelectResult == SOCKET_ERROR)
        {
            if (!isRunning)
                break;
            std::cerr << "[CRITICAL ERROR] TcpServer: Listen select failed. Error code: " << WSAGetLastError() << "\n";
            return; // Thoát hẳn khỏi run() phòng vệ lặp vô hạn
        }

        if (listenSelectResult > 0)
        {
            if (!FD_ISSET(listenSocket, &listenReadSet))
                continue;
        }

        std::cout << "\n[INFO] TcpServer: Ready and welcoming a new Control Application connection...\n";

        // Chấp nhận kết nối (Chạy lập tức vì listenSocket đã được select xác nhận có tín hiệu)
        clientSocket = accept(listenSocket, nullptr, nullptr);

        if (clientSocket == INVALID_SOCKET)
        {
            if (!isRunning)
                break;
            std::cerr << "[ERROR] TcpServer: Accept failed with error code: " << WSAGetLastError() << "\n";
            continue;
        }

        std::cout << "[CONNECTED] A new Control Application has linked to the server!\n";

        // --- 2. BẢO VỆ VÒNG LẶP TRONG: SELECT() CHO CLIENTSOCKET ---
        // VÒNG LẶP TRONG: Đọc và xử lý liên tục chuỗi lệnh của riêng Client hiện tại
        while (isRunning)
        {
            fd_set clientReadSet;
            FD_ZERO(&clientReadSet);
            FD_SET(clientSocket, &clientReadSet);

            timeval clientTimeout{0, 500000}; // Timeout 500ms

            int clientSelectResult = select(0, &clientReadSet, nullptr, nullptr, &clientTimeout);

            // Kiểm tra cờ trạng thái Server ngay sau khi select trả quyền điều khiển
            if (!isRunning)
            {
                break; // Thoát vòng lặp trong nếu phát hiện lệnh shutdown
            }

            if (clientSelectResult == 0)
            {
                // Timeout: Client chưa gửi dữ liệu gì trong 0.5 giây qua
                // Quay lại đầu vòng lặp trong để tiếp tục kiểm tra cờ isRunning
                continue;
            }

            if (clientSelectResult == SOCKET_ERROR)
            {
                std::cerr << "[ERROR] TcpServer: Client select failed with error code: " << WSAGetLastError() << "\n";
                break; // Thoát vòng lặp trong để giải phóng phiên client lỗi
            }

            if (!FD_ISSET(clientSocket, &clientReadSet))
            {
                continue;
            }

            // --- CHỈ GỌI RECV() KHI SOCKET ĐÃ SẴN SÀNG ĐỌC DỮ LIỆU THỰC SỰ ---
            char recvBuffer[512]{}; // Đảm bảo bộ đệm mảng ký tự sạch sẽ
            int bytesReceived = recv(clientSocket, recvBuffer, sizeof(recvBuffer) - 1, 0);

            if (bytesReceived > 0)
            {
                recvBuffer[bytesReceived] = '\0';
                std::string command(recvBuffer);

                // Cắt bỏ ký tự xuống dòng rác \n và \r ở cuối chuỗi
                while (!command.empty() && (command.back() == '\n' || command.back() == '\r'))
                {
                    command.pop_back();
                }

                if (command.empty())
                    continue;

                std::cout << "[RECEIVED COMMAND]: " << command << "\n";

                // Gọi bộ xử lý lệnh tương tác với thiết bị ảo
                std::string response = processCommand(device, command);

                int bytesSent = send(clientSocket, response.c_str(), static_cast<int>(response.length()), 0);
                if (bytesSent == SOCKET_ERROR)
                {
                    std::cerr << "[ERROR] TcpServer: Send failed with error code: " << WSAGetLastError() << "\n";
                    break; // Thoát vòng lặp trong
                }
            }
            else if (bytesReceived == 0)
            {
                std::cout << "[INFO] TcpServer: Current Client disconnected cleanly.\n";
                break; // Thoát vòng lặp trong
            }
            else
            {
                std::cerr << "[ERROR] TcpServer: Connection lost with current client. Error: " << WSAGetLastError() << "\n";
                break; // Thoát vòng lặp trong
            }
        } // --- KẾT THÚC VÒNG LẶP TRONG ---

        // Tiến hành dọn dẹp đóng tài nguyên của client hiện tại trước khi lên vòng lặp ngoài đón client mới
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        std::cout << "[INFO] TcpServer: Session ended cleanly. Clearing socket resource.\n";
    } // --- KẾT THÚC VÒNG LẶP NGOÀI ---
}

// 6. Hàm giải phóng tài nguyên mạng khi dừng Server
void TcpServer::stop()
{
    // 1. Yêu cầu server dừng hẳn các vòng lặp mạng
    isRunning = false;

    // 2. Dừng luồng ngầm của bộ giám sát thiết bị ảo an toàn (gọi lệnh join)
    monitor.stop();

    // 3. Đóng an toàn các Socket vật lý sau khi server thread đã kết thúc hoàn toàn
    if (clientSocket != INVALID_SOCKET)
    {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }

    if (listenSocket != INVALID_SOCKET)
    {
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
    }

    // 4. Thực hiện Cleanup giải phóng Winsock đúng một lần dựa trên cờ winsockInitialized
    if (winsockInitialized)
    {
        WSACleanup();
        winsockInitialized = false; // Hạ cờ bảo vệ phòng vệ gọi trùng lặp

        std::cout << "[INFO] TcpServer stopped and cleaned up.\n";
    }
}

void TcpServer::requestStop()
{
    // Đánh sập cờ hiệu một cách an toàn và nguyên tử tuyệt đối
    isRunning = false;
}
