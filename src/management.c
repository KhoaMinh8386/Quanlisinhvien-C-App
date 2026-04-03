/* management.c — Module CRUD sinh viên */
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "student.h"
#include "management.h"
#include "academic.h"

/* Tìm sinh viên theo ID, trả về index hoặc -1 nếu không tồn tại */
int find_student_by_id(Student arr[], int count, int id) {
    for (int i = 0; i < count; i++)
        if (arr[i].id == id) return i;
    return -1;
}

/* Thêm sinh viên vào mảng */
int add_student(Student arr[], int *count, Student s) {
    if (*count >= MAX_STUDENTS) return 0;
    if (find_student_by_id(arr, *count, s.id) != -1) return 0;
    
    /* Tính toán nếu chưa có subject_count */
    if (s.subject_count > 0) {
        calculate_student_gpa(&s);
        calculate_full_tuition(&s);
    } else {
        calculate_student_gpa(&s);
        calculate_full_tuition(&s);
    }
    arr[*count] = s;
    (*count)++;
    return 1;
}

/* Lưu tất cả sinh viên vào file nhị phân */
void save_to_file(Student arr[], int count) {
    FILE *f = fopen("students.dat", "wb");
    if (!f) return;
    fwrite(&count, sizeof(int), 1, f);
    if (count > 0) fwrite(arr, sizeof(Student), count, f);
    fclose(f);
}

/* Tải sinh viên từ file nhị phân */
void load_from_file(Student arr[], int *count) {
    FILE *f = fopen("students.dat", "rb");
    if (!f) return;
    if (fread(count, sizeof(int), 1, f) == 1) {
        if (*count > MAX_STUDENTS) *count = MAX_STUDENTS;
        if (*count > 0) fread(arr, sizeof(Student), *count, f);
    }
    fclose(f);
}
