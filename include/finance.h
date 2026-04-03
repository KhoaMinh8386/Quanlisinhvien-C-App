#ifndef FINANCE_H
#define FINANCE_H

#include "student.h"

/* Tính từ credits × 1,200,000 (VNĐ) */
void calculate_tuition(Student *s);

/* Áp dụng học bổng dựa trên GPA_4 */
void apply_scholarship(Student *s);

/* final_payable = max(0, tuition_base - scholarship) */
void calculate_final(Student *s);

/* Trạng thái học bổng cho GUI badge */
ScholarshipStatus get_scholarship_status(const Student *s);

#endif /* FINANCE_H */
