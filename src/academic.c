/* academic.c — Module tinh GPA và hoc phi */
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "student.h"
#include "academic.h"

/* ====================================================
 * DU LIEU MAC DINH
 * ==================================================== */
SubjectCatalog DEFAULT_SUBJECTS[10] = {
    {101, L"MATH101", L"Toán Cao Cấp A1",       3},
    {102, L"PHYS101", L"Vật Lý Đại Cương",       3},
    {103, L"PROG101", L"Lập Trình C Cơ Bản",     3},
    {104, L"ENG101",  L"Tiếng Anh Cơ Bản",       2},
    {105, L"CHEM101", L"Hóa Đại Cương",           2},
    {106, L"PHIL101", L"Triết Học Mác-Lênin",     2},
    {107, L"ECON101", L"Kinh Tế Vi Mô",           3},
    {108, L"STAT101", L"Xác Suất Thống Kê",       3},
    {109, L"DB101",   L"Cơ Sở Dữ Liệu",           3},
    {110, L"NET101",  L"Mạng Máy Tính",            2},
};
int DEFAULT_SUBJECT_COUNT = 10;

ClassInfo DEFAULT_CLASSES[4] = {
    {1, L"CNTT-A", L"CNTT Lớp A", L"Công nghệ thông tin", 1200000.0f},
    {2, L"CNTT-B", L"CNTT Lớp B", L"Công nghệ thông tin", 1200000.0f},
    {3, L"KT-A",   L"Kế Toán A",  L"Kế toán",             1100000.0f},
    {4, L"QT-A",   L"Quản Trị A", L"Quản trị kinh doanh",  1050000.0f}
};
int DEFAULT_CLASS_COUNT = 4;

void calculate_subject_gpa(SubjectScore *sub) {
    sub->gpa_10 = sub->score_process * 0.2f + sub->score_midterm * 0.3f + sub->score_final * 0.5f;
    if      (sub->gpa_10 >= 9.0f) { sub->gpa_4 = 4.0f; wcscpy(sub->grade, L"A+"); }
    else if (sub->gpa_10 >= 8.5f) { sub->gpa_4 = 3.7f; wcscpy(sub->grade, L"A");  }
    else if (sub->gpa_10 >= 8.0f) { sub->gpa_4 = 3.5f; wcscpy(sub->grade, L"B+"); }
    else if (sub->gpa_10 >= 7.0f) { sub->gpa_4 = 3.0f; wcscpy(sub->grade, L"B");  }
    else if (sub->gpa_10 >= 6.0f) { sub->gpa_4 = 2.0f; wcscpy(sub->grade, L"C");  }
    else if (sub->gpa_10 >= 4.0f) { sub->gpa_4 = 1.0f; wcscpy(sub->grade, L"D");  }
    else                           { sub->gpa_4 = 0.0f; wcscpy(sub->grade, L"F");  }
}

void calculate_student_gpa(Student *s) {
    float w4 = 0, s10 = 0; int cr = 0; s->fail_count = 0;
    for (int i=0; i<s->subject_count; i++) {
        calculate_subject_gpa(&s->subjects[i]);
        w4 += s->subjects[i].gpa_4 * s->subjects[i].credits;
        s10 += s->subjects[i].gpa_10 * s->subjects[i].credits;
        cr += s->subjects[i].credits;
        if (s->subjects[i].gpa_4 < 1.0f) s->fail_count++;
    }
    s->total_credits = cr;
    s->gpa_4_avg = (cr>0) ? w4/cr : 0;
    s->gpa_10_avg = (cr>0) ? s10/cr : 0;
    if      (s->gpa_4_avg >= 3.6f) wcscpy(s->rank, L"Xuất Sắc");
    else if (s->gpa_4_avg >= 3.2f) wcscpy(s->rank, L"Giỏi");
    else if (s->gpa_4_avg >= 2.5f) wcscpy(s->rank, L"Khá");
    else if (s->gpa_4_avg >= 1.0f) wcscpy(s->rank, L"Trung Bình");
    else                           wcscpy(s->rank, L"Yếu");
}

void calculate_full_tuition(Student *s) {
    float rate = 1200000.0f;
    for(int i=0; i<DEFAULT_CLASS_COUNT; i++) if(DEFAULT_CLASSES[i].id == s->class_id) rate = DEFAULT_CLASSES[i].tuition_per_credit;
    s->tuition_base = (float)s->total_credits * rate;
    if      (s->gpa_4_avg >= 3.6f) s->tuition_scholarship = 10000000.0f;
    else if (s->gpa_4_avg >= 3.2f) s->tuition_scholarship =  5000000.0f;
    else                           s->tuition_scholarship =         0.0f;
    s->tuition_total = s->tuition_base - s->tuition_scholarship + s->tuition_penalty;
    if (s->tuition_total < 0) s->tuition_total = 0;
}

ScholarshipStatus get_scholarship_status(const Student *s) {
    if (s->gpa_4_avg >= 3.6f) return SCHOLARSHIP_FULL;
    if (s->gpa_4_avg >= 3.2f) return SCHOLARSHIP_NORMAL;
    return SCHOLARSHIP_NONE;
}

void format_currency(long amount, char* out_buf) {
    sprintf(out_buf, "%ld VND", amount);
}
