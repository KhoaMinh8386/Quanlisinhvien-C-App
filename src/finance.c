#include "../include/finance.h"

void calculate_tuition(Student *s) {
    if (s == NULL) return;
    s->tuition_base = (long)s->credits * 1200000L;
}

void apply_scholarship(Student *s) {
    if (s == NULL) return;
    if (s->gpa_4 >= 3.6f) {
        s->scholarship = 10000000L;
    } else if (s->gpa_4 >= 3.2f) {
        s->scholarship = 5000000L;
    } else {
        s->scholarship = 0L;
    }
}

void calculate_final(Student *s) {
    if (s == NULL) return;
    if (s->scholarship > s->tuition_base) {
        s->final_payable = 0;
    } else {
        s->final_payable = s->tuition_base - s->scholarship;
    }
}

ScholarshipStatus get_scholarship_status(const Student *s) {
    if (s == NULL) return SCHOLARSHIP_NONE;
    if (s->gpa_4 >= 3.6f) return SCHOLARSHIP_FULL;
    if (s->gpa_4 >= 3.2f) return SCHOLARSHIP_NORMAL;
    return SCHOLARSHIP_NONE;
}
