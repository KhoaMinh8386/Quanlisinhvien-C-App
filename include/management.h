#ifndef MANAGEMENT_H
#define MANAGEMENT_H

#include "student.h"

int  find_student_by_id(Student arr[], int count, int id);
int  add_student(Student arr[], int *count, Student s);
void save_to_file(Student arr[], int count);
void load_from_file(Student arr[], int *count);

#endif
