#include <assert.h>
#include <stddef.h>
#include "../include/student.h"
#include "../include/academic.h"
#include "../include/finance.h"

/* Khai báo hàm nondet của CBMC */
int   nondet_int(void);
float nondet_float(void);

void verify_logic(void) {
    Student s;

    /* === TIỀN ĐIỀU KIỆN (Preconditions) === */
    s.credits         = nondet_int();
    s.score_attendance = nondet_float();
    s.score_midterm   = nondet_float();
    s.score_final     = nondet_float();

    /* Ràng buộc phạm vi nhập liệu thực tế */
    __CPROVER_assume(s.credits >= 0 && s.credits <= 30);
    __CPROVER_assume(s.score_attendance >= 0.0f && s.score_attendance <= 10.0f);
    __CPROVER_assume(s.score_midterm    >= 0.0f && s.score_midterm    <= 10.0f);
    __CPROVER_assume(s.score_final      >= 0.0f && s.score_final      <= 10.0f);

    /* === THỰC HIỆN TÍNH TOÁN === */
    calculate_gpa(&s);
    calculate_tuition(&s);
    apply_scholarship(&s);
    calculate_final(&s);

    /* === THUỘC TÍNH CẦN KIỂM CHỨNG (Properties) === */

    /* P1: GPA thang 4 không vượt quá 4.0 */
    assert(s.gpa_4 >= 0.0f && s.gpa_4 <= 4.0f);

    /* P2: GPA thang 10 không vượt quá 10.0 */
    assert(s.gpa_10 >= 0.0f && s.gpa_10 <= 10.0f);

    /* P3: Học phí thực đóng không bao giờ âm */
    assert(s.final_payable >= 0);

    /* P4: Học bổng không vượt quá học phí gốc hoặc kết quả bằng 0 */
    assert(s.scholarship <= s.tuition_base || s.final_payable == 0);

    /* P5: Kiểm tra tràn số */
    int n = nondet_int();
    __CPROVER_assume(n >= 1 && n <= MAX_STUDENTS);
    
    long total = (long)s.final_payable * n;
    assert(total >= 0);
}
