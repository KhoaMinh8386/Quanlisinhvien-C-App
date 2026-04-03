#ifndef DLG_STUDENT_H
#define DLG_STUDENT_H
#include "student.h"

/* Mở dialog thêm/sửa sinh viên, trả về TRUE nếu người dùng đã Lưu */
BOOL ShowStudentDialog(HWND hParent, Student* s, BOOL isEdit, HINSTANCE hInst);

#endif
