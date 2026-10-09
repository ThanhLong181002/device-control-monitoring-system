#include <ws2tcpip.h>
#include "TcpClient.h"
#include <iostream>

// Constructor sử dụng Member Initializer List để khởi tạo toàn bộ thuộc tính
TcpClient::TcpClient(const std::string &ip, int port)
    : serverIp(ip),
      serverPort(port),
      connectSocket(INVALID_SOCKET), // Trên Windows, giá trị mặc định an toàn cho socket là INVALID_SOCKET
      isConnected(false)
{
    // Bỏ trống bên trong thân hàm vì tất cả dữ liệu đã được cấu hình ở danh sách khởi tạo phía trên
}

// Destructor tự động dọn dẹp kết nối mạng theo chuẩn RAII
TcpClient::~TcpClient()
{
    // Đảm bảo khi đối tượng Client bị hủy, kết nối sẽ được ngắt một cách an toàn
    disconnect();
}

// --- Các hàm thành viên tạm thời để trống làm khung sườn cho bước tiếp theo ---

bool TcpClient::initWinsock()
{
    WSADATA wsaData;

    // Yêu cầu sử dụng Winsock phiên bản 2.2
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        // Trên Windows, nếu WSAStartup thất bại, hàm sẽ trả về trực tiếp mã lỗi (error code)
        std::cerr << "[ERROR] TcpClient: WSAStartup failed with error code: " << result << "\n";
        return false;
    }

    return true;
}

bool TcpClient::connectToServer()
{
    // 1. Khởi tạo thư viện Winsock
    if (!initWinsock())
    {
        return false;
    }

    // 2. Tạo Socket kết nối (socket())
    connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET)
    {
        std::cerr << "[ERROR] TcpClient: Socket creation failed with error: " << WSAGetLastError() << "\n";
        WSACleanup();
        return false;
    }

    // 3. Cấu hình cấu trúc địa chỉ cấu hình sockaddr_in dựa trên biến thành viên
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort); // Chuyển đổi port sang định dạng mạng (Network Byte Order)

    // Chuyển đổi chuỗi IP thành địa chỉ binary (Sử dụng member serverIp)
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0)
    {
        std::cerr << "[ERROR] TcpClient: Invalid IP address or Address family not supported.\n";
        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
        WSACleanup();
        return false;
    }

    // 4. Tiến hành kết nối tới Server (connect())
    int result = connect(connectSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "[ERROR] TcpClient: Connection to " << serverIp << ":" << serverPort
                  << " failed with error: " << WSAGetLastError() << "\n";
        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
        WSACleanup();
        return false;
    }

    // 5. Kết nối thành công, cập nhật trạng thái hệ thống
    isConnected = true;
    std::cout << "[SUCCESS] Connected to Server at " << serverIp << ":" << serverPort << "\n";
    return true;
}

bool TcpClient::sendMessage(const std::string &message)
{
    // 1. Điều kiện: Nếu chưa kết nối thành công thì không cho phép gửi
    if (!isConnected || connectSocket == INVALID_SOCKET)
    {
        std::cerr << "[WARNING] TcpClient: Cannot send message. Client is not connected to any server.\n";
        return false;
    }

    // 2. Gọi hàm send() của Winsock
    // Ép kiểu static_cast<int> để loại bỏ hoàn toàn cảnh báo warning C4267 trên Windows
    int bytesSent = send(connectSocket, message.c_str(), static_cast<int>(message.length()), 0);

    // 3. Kiểm tra lỗi truyền tải (SOCKET_ERROR)
    if (bytesSent == SOCKET_ERROR)
    {
        std::cerr << "[ERROR] TcpClient: Send failed with error code: " << WSAGetLastError() << "\n";
        return false;
    }

    // Log nhẹ để kiểm tra số lượng byte thực tế được đẩy đi
    // std::cout << "[INFO] Sent " << bytesSent << " bytes to server.\n";

    return true;
}

std::string TcpClient::receiveResponse()
{
    // 1. Kiểm tra điều kiện kết nối
    if (!isConnected || connectSocket == INVALID_SOCKET)
    {
        std::cerr << "[WARNING] TcpClient: Cannot receive data. Client is not connected.\n";
        return "";
    }

    // 2. Khởi tạo mảng buffer để hứng dữ liệu
    char buffer[512];

    // 3. Gọi hàm recv() của Winsock (Hàm này sẽ block luồng cho đến khi có dữ liệu đổ về)
    int bytesReceived = recv(connectSocket, buffer, sizeof(buffer) - 1, 0);

    // 4. Phân tích kết quả trả về từ hàm recv()
    if (bytesReceived > 0)
    {
        // Trường hợp 1: Nhận dữ liệu thành công
        buffer[bytesReceived] = '\0'; // Thêm ký tự null để đảm bảo chuỗi kết thúc an toàn
        return std::string(buffer);   // Chuyển đổi mảng char sang std::string để trả về ngoài
    }
    else if (bytesReceived == 0)
    {
        // Trường hợp 2: Server đóng kết nối chủ động (Clean disconnect)
        std::cout << "[INFO] TcpClient: Server closed connection cleanly.\n";
        isConnected = false;
        return "";
    }
    else
    {
        // Trường hợp 3: Lỗi trong quá trình nhận dữ liệu (SOCKET_ERROR)
        std::cerr << "[ERROR] TcpClient: Recv failed with error code: " << WSAGetLastError() << "\n";
        return "";
    }
}

void TcpClient::disconnect()
{
    // Tạm thời in ra log để chúng ta kiểm tra vòng đời hoạt động của Class
    std::cout << "[INFO] TcpClient destructor called. Disconnecting cleanly...\n";
}
