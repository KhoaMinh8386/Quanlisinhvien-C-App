#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "../student_manager_gui/include/student.h"
#include "../student_manager_gui/include/academic.h"

int main() {
    printf("Running test_gpa...\n");
    SubjectScore sub;
    memset(&sub, 0, sizeof(sub));
    
    // Case 1: Học lực khá (B+)
    sub.score_process = 8.0f;
    sub.score_midterm = 8.0f;
    sub.score_final = 8.0f;
    calculate_subject_gpa(&sub);
    assert(fabs(sub.gpa_10 - 8.0f) < 0.01f);
    assert(fabs(sub.gpa_4 - 3.5f) < 0.01f);
    assert(wcscmp(sub.grade, L"B+") == 0);
    
    // Case 2: Điểm tuyệt đối (Biên)
    sub.score_process = 10.0f; 
    sub.score_midterm = 10.0f; 
    sub.score_final = 10.0f;
    calculate_subject_gpa(&sub);
    assert(fabs(sub.gpa_10 - 10.0f) < 0.01f);
    assert(fabs(sub.gpa_4 - 4.0f) < 0.01f);

    // Case 3: Trượt (Điểm F)
    sub.score_process = 3.0f; 
    sub.score_midterm = 3.0f; 
    sub.score_final = 3.0f;
    calculate_subject_gpa(&sub);
    assert(fabs(sub.gpa_10 - 3.0f) < 0.01f);
    assert(fabs(sub.gpa_4 - 0.0f) < 0.01f);

    printf("-> test_gpa PASSED!\n\n");
    return 0;
}
