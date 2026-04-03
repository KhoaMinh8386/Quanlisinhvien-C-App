# 🎓 Hệ Thống Quản Lý Sinh Viên & Tài Chính (Pro v2.5)

Dự án Hệ thống Quản lý Sinh viên chuyên nghiệp được xây dựng bằng ngôn ngữ **C thuần** kết hợp với giao diện **Win32 API** và kỹ thuật đồ hoạ **GDI (Graphics Device Interface)**. Bản nâng cấp Pro v2.5 mang lại một trải nghiệm UI/UX hoàn toàn mới với bảng màu hiện đại, sidebar thông minh, và các biểu đồ báo cáo vẽ tay bằng GDI không hề nhấp nháy nhờ kỹ thuật **Double-Buffering**.

---

## 🌟 Tính Năng Nổi Bật

### 1. 💼 Quản Lý Sinh Viên Chuyên Nghiệp
*   **Thêm/Sửa/Xóa Sinh viên:** Quản lý thông tin chi tiết với đầy đủ các trường dữ liệu (Họ tên, Ngày sinh, Lớp, Giới tính, Quê quán...).
*   **Trang Chi Tiết Thông Minh:** Kích đúp chuột vào sinh viên bất kỳ để gọi ra cửa sổ (Dialog) chi tiết với đồ họa thẻ (Card Layout).
*   **Tìm kiếm:** Tích hợp thanh tìm kiếm sinh viên theo tên hoặc ID tích hợp trực tiếp trên thanh công cụ.

### 2. 📚 Thuật Toán Học Vụ Tự Động (GPA & Xếp Loại)
*   Hỗ trợ thêm/bớt động số lượng môn học cho mỗi sinh viên.
*   **Tự động tính toán:** Tính điểm quá trình, giữa kỳ, cuối kỳ thành GPA hệ 10 và quy đổi sang điểm chữ (A, B, C, D, F) cùng GPA hệ 4.0.
*   **Validation:** Chặn ngay lập tức nếu người dùng nhập điểm sai thực tế (nhỏ hơn 0.0 hoặc lớn hơn 10.0).

### 3. 💰 Phân Tích & Quản Trị Tài Chính
*   Tự động tính **Học phí gốc** dựa trên số tín chỉ đã đăng ký.
*   Hệ thống học bổng thông minh: Sinh viên Giỏi/Xuất Sắc tự động được cấp học bổng giảm trừ vào học phí gốc.
*   Quản lý **Trạng thái đóng phí** (Đã đóng / Chưa đóng) với các huy hiệu (Badge) màu sắc nổi bật.

### 4. 📊 Dashboard Báo Cáo GDI Đa Sắc
*   Bộ thẻ thống kê chỉ số cốt lõi: Tổng Sinh Viên, Tổng Thu Thực Tế, GPA Trung Bình, Số Lượng Có Học Bổng.
*   **Khung biểu đồ (Bar Chart):** Cột biểu đồ nhiều màu sắc báo cáo tình hình học lực (Xuất sắc, Giỏi, Khá, Trung bình, Yếu) được vẽ hoàn toàn bằng tọa độ hình học (GDI) đảm bảo tốc độ phản hồi tính bằng mili-giây.

### 5. 🛠 Tiện Ích Mở Rộng
*   **Xuất CSV:** Kết xuất dữ liệu báo cáo ra file Excel (`.csv`) với chuẩn mã hóa UTF-8 BOM, không lỗi font tiếng Việt.
*   **Lưu trữ nhị phân:** Hệ thống file dữ liệu nội tại (`students.dat`) lưu trữ cực nhanh.

---

## 🎨 Trải Nghiệm Giao Diện (UI/UX)
*   **Màu sắc:** Phủ lên sắc xanh Indigo và Slate 900 theo phong cách phẳng (Flat Design) và tối giản.
*   **Phông chữ Unicode (Segoe UI):** Toàn bộ giao diện 100% hiển thị tiếng Việt có dấu, triệt tiêu mọi vấn đề lỗi Text do Win32 API gây ra nhờ ép hàm `SendMessage(..., WM_SETFONT, ...)`.
*   **Flicker-Free:** Kỹ thuật vẽ hai màng (Double Buffer Bitmap) loại bỏ hoàn toàn hiện tượng vỡ khung hình khi thao tác và chuyển trang.

---

## 📁 Cấu Trúc Thư Mục

```text
student_manager_gui/
├── include/
│   ├── student.h       # (Data Models) Khai báo Struct Sinh viên & Tài chính
│   ├── academic.h      # Khai báo hàm hệ thống học vụ
│   ├── dlg_student.h   # Khai báo hệ Dialog Form (Thêm/Sửa)
│   ├── dlg_detail.h    # Khai báo hệ Dialog Chi tiết (Detail View)
│   └── management.h    # Khai báo Logic Quản trị tập tin
├── src/
│   ├── main_win32.c    # Entry point WinMain, Sidebar & Vẽ Dashboard GDI
│   ├── dlg_student.c   # Logic Form nhập liệu và Check Validation Điểm (0-10)
│   ├── dlg_detail.c    # GDI Graphic cho bảng điểm chi tiết của cá nhân SV
│   ├── academic.c      # Dữ liệu Hard-code & Cấu trúc thuật toán GPA
│   └── management.c    # Engine Đọc/Ghi/Lưu file Database
├── Makefile_win32      # Script Build tiêu chuẩn
└── students.dat        # File CSDL nhị phân lưu danh sách SV
```

---

## ⚙️ Hướng Dẫn Cài Đặt & Biên Dịch (Compile)

Dự án sử dụng trình biên dịch **MinGW GCC** cho môi trường Windows.

**Cách 1: Sử dụng Makefile**
Mở Terminal/Powershell nằm ở thư mục root dự án và chạy:
```bash
mingw32-make -f Makefile_win32
.\student_manager.exe
```

**Cách 2: Gọi lệnh GCC trực tiếp**
Trong trường hợp không dùng Make, bạn copy và chạy trực tiếp lệnh sau:
```bash
gcc -Wall -Wextra -std=c99 -Iinclude -o student_manager.exe src/main_win32.c src/academic.c src/management.c src/dlg_student.c src/dlg_detail.c -lcomctl32 -lgdi32 -lcomdlg32 -luser32 -mwindows -finput-charset=UTF-8

.\student_manager.exe
```

---

*Hệ Thống Quản Lý Sinh Viên Pro v2.5 — Sự kết hợp giữa tốc độ C và kiến trúc giao diện Cổ điển & Hiện đại.*
