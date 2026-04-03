#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "../student_manager_gui/include/student.h"
#include "../student_manager_gui/include/academic.h"

int main() {
    printf("Running test_tuition...\n");
    Student s;
    memset(&s, 0, sizeof(s));
    
    // Giả sử class_id = 1 (CNTT) học phí là 1.200.000 VNĐ / tín chỉ
    s.class_id = 1; 
    s.total_credits = 10;
    
    // Case 1: Không có học bổng
    s.gpa_4_avg = 2.5f;
    calculate_full_tuition(&s);
    // Gốc = 10 * 1.200.000 = 12.000.000
    assert(fabs(s.tuition_base - 12000000.0f) < 1.0f);
    assert(fabs(s.tuition_scholarship - 0.0f) < 1.0f);
    assert(fabs(s.tuition_total - 12000000.0f) < 1.0f);

    // Case 2: Học bổng Xuất Sắc (10.000.000)
    s.gpa_4_avg = 3.8f;
    calculate_full_tuition(&s);
    assert(fabs(s.tuition_scholarship - 10000000.0f) < 1.0f);
    assert(fabs(s.tuition_total - 2000000.0f) < 1.0f);
    
    // Đảm bảo không âm nếu học bổng > học phí gốc
    s.total_credits = 5; // 6.000.000
    calculate_full_tuition(&s);
    assert(s.tuition_total >= 0.0f);

    printf("-> test_tuition PASSED!\n\n");
    return 0;
}
