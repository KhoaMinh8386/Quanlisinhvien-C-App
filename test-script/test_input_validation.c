#include <stdio.h>
#include <assert.h>

// Hàm giả lập validate từ Form
int validate_score(float s) {
    if (s < 0.0f || s > 10.0f) return 0;
    return 1;
}

int main() {
    printf("Running test_input_validation...\n");
    
    // Case 1: Hợp lệ
    assert(validate_score(5.0f) == 1);
    assert(validate_score(0.0f) == 1);
    assert(validate_score(10.0f) == 1);
    
    // Case 2: Không hợp lệ
    assert(validate_score(-1.0f) == 0);
    assert(validate_score(11.0f) == 0);
    assert(validate_score(-0.01f) == 0);
    
    printf("-> test_input_validation PASSED!\n\n");
    return 0;
}
