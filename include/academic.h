#ifndef ACADEMIC_H
#define ACADEMIC_H

#include "student.h"

/* Dữ liệu danh mục dùng chung */
extern SubjectCatalog DEFAULT_SUBJECTS[10];
extern int DEFAULT_SUBJECT_COUNT;
extern ClassInfo DEFAULT_CLASSES[4];
extern int DEFAULT_CLASS_COUNT;

void calculate_subject_gpa(SubjectScore *sub);
void calculate_student_gpa(Student *s);
void calculate_full_tuition(Student *s);
void format_currency(long amount, char* out_buf);

ScholarshipStatus get_scholarship_status(const Student *s);

#endif
