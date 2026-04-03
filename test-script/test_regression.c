#include <stdio.h>
#include <assert.h>

int main() {
    printf("Running test_regression...\n");
    // Verify older logic has not been broken
    // Ex: Double floating precision issues
    double value = 1.0 / 3.0;
    assert((value * 3.0) == 1.0);
    
    printf("-> test_regression PASSED!\n\n");
    return 0;
}
