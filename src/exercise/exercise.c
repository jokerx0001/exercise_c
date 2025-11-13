//
//  exercise.c
//  exercise
//
//  Created by jokerx on 2025/11/10.
//

#include "exercise.h"
//void test();

void run_exercise(void) {
}

void exercise_callback(void) {
    int (*f)(int, int);
    f = &plus;
    int result = callback(1, 2, f);
    printf("result=%d\n", result);
}

int callback(int a, int b, int (*p)(int, int)) {
    return p(a, b);
}

int plus(int a, int b) {
    return a + b;
}

// 不指定void时候，可以往无参数的函数里随意传递参数，且编译器无法检查不报错，在某些严格的环境可能在执行时候报错
//    test(123123123);
//void test() {}
