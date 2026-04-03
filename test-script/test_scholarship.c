#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../student_manager_gui/include/student.h"
#include "../student_manager_gui/include/academic.h"

int main() {
    printf("Running test_scholarship...\n");
    Student s;
    memset(&s, 0, sizeof(s));
    
    // Case 1: Học bổng Xuất Sắc (Full) - GPA 3.6+
    s.gpa_4_avg = 3.8f;
    ScholarshipStatus st = get_scholarship_status(&s);
    assert(st == SCHOLARSHIP_FULL);
    
    // Case 2: Học bổng Giỏi (Normal) - GPA 3.2+
    s.gpa_4_avg = 3.4f;
    st = get_scholarship_status(&s);
    assert(st == SCHOLARSHIP_NORMAL);
    
    // Case 3: Không học bổng - GPA < 3.2
    s.gpa_4_avg = 2.5f;
    st = get_scholarship_status(&s);
    assert(st == SCHOLARSHIP_NONE);
    
    printf("-> test_scholarship PASSED!\n\n");
    return 0;
}
