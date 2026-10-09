#pragma once

// Hủy định nghĩa tuyệt đối macro trùng tên của Windows để bảo vệ hằng số enum
#ifdef ERROR
#undef ERROR
#endif

enum class DeviceState
{
    IDLE,
    RUNNING,
    ERROR, // Chữ ERROR lúc này đã được an toàn tuyệt đối
    EMERGENCY_STOP
};

// Đảm bảo sau khi khai báo xong, nếu thư viện khác của Windows cần macro ERROR, ta hủy tiếp để tránh rò rỉ rác
#ifdef ERROR
#undef ERROR
#endif
