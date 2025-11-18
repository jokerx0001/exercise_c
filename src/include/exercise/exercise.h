//
//  exercise.h
//  exercise
//
//  Created by jokerx on 2025/11/10.
//

#ifndef exercise_h
#define exercise_h

#include <stdio.h>
#include <string.h>

void run_exercise(void);
void test_string(void);
void exercise_callback(void);
int callback(int a, int b, int (*p)(int, int));
int plus(int a, int b);
void string_func(void);

#endif /* exercise_h */
