#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../student_manager_gui/include/student.h"
#include "../student_manager_gui/include/academic.h"

int main() {
    printf("Running test_boundary...\n");
    Student s;
    memset(&s, 0, sizeof(s));
    
    // Boundary Testing: GPA học bổng
    s.gpa_4_avg = 3.2f;
    assert(get_scholarship_status(&s) == SCHOLARSHIP_NORMAL);
    
    s.gpa_4_avg = 3.19f;
    assert(get_scholarship_status(&s) == SCHOLARSHIP_NONE);
    
    s.gpa_4_avg = 3.6f;
    assert(get_scholarship_status(&s) == SCHOLARSHIP_FULL);
    
    s.gpa_4_avg = 3.59f;
    assert(get_scholarship_status(&s) == SCHOLARSHIP_NORMAL);
    
    printf("-> test_boundary PASSED!\n\n");
    return 0;
}
