#ifndef STUDENT_H
#define STUDENT_H

#include <windows.h>

#define MAX_SUBJECTS    15
#define MAX_STUDENTS   200
#define MAX_CLASSES     20

typedef enum {
    SCHOLARSHIP_NONE = 0,
    SCHOLARSHIP_NORMAL,
    SCHOLARSHIP_FULL
} ScholarshipStatus;

typedef struct {
    int     catalog_id;
    wchar_t code[20];
    wchar_t name[100];
    int     credits;
    float   score_process;
    float   score_midterm;
    float   score_final;
    float   gpa_10;
    float   gpa_4;
    wchar_t grade[5];
} SubjectScore;

typedef struct {
    int     id;
    wchar_t code[20];
    wchar_t name[100];
    int     credits;
} SubjectCatalog;

typedef struct {
    int     id;
    wchar_t code[20];
    wchar_t name[50];
    wchar_t major[50];
    float   tuition_per_credit;
} ClassInfo;

typedef struct {
    int     id;
    wchar_t name[100];
    wchar_t birth_date[20];
    wchar_t gender[10];
    wchar_t phone[20];
    wchar_t email[100];
    wchar_t class_name[50];
    int     class_id;
    int     semester;

    SubjectScore subjects[MAX_SUBJECTS];
    int          subject_count;
    int          total_credits;
    float        gpa_10_avg;
    float        gpa_4_avg;
    wchar_t      rank[20];
    int          fail_count;

    float tuition_base;
    float tuition_scholarship;
    float tuition_penalty;
    float tuition_total;
    int   tuition_status;        /* 0: Chua dong, 1: Da dong */
    int   payment_status;
    wchar_t payment_date[20];
} Student;

#endif
