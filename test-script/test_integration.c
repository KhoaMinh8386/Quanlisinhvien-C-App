#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "../student_manager_gui/include/student.h"
#include "../student_manager_gui/include/academic.h"

int main() {
    printf("Running test_integration...\n");
    Student s;
    memset(&s, 0, sizeof(s));
    s.id = 1;
    s.class_id = 1; // CNTT
    s.subject_count = 2;
    
    // Môn 1
    wcscpy(s.subjects[0].name, L"Toán");
    s.subjects[0].credits = 3;
    s.subjects[0].score_process = 9.0f;
    s.subjects[0].score_midterm = 9.0f;
    s.subjects[0].score_final = 9.0f;

    // Môn 2
    wcscpy(s.subjects[1].name, L"Lý");
    s.subjects[1].credits = 2;
    s.subjects[1].score_process = 8.0f;
    s.subjects[1].score_midterm = 8.0f;
    s.subjects[1].score_final = 8.0f;

    // Chạy Engine
    calculate_student_gpa(&s);
    calculate_full_tuition(&s);

    // Kiểm thử kết quả Tích hợp
    assert(s.total_credits == 5);
    // GPA môn 1: 9.0 (A+ -> 4.0) | GPA môn 2: 8.0 (B+ -> 3.5)
    // Trung bình 4.0 = (4.0*3 + 3.5*2)/5 = 19/5 = 3.8
    assert(fabs(s.gpa_4_avg - 3.8f) < 0.01f);
    assert(wcscmp(s.rank, L"Xuất Sắc") == 0);

    printf("-> test_integration PASSED!\n\n");
    return 0;
}
