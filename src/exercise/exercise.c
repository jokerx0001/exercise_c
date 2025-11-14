//
//  exercise.c
//  exercise
//
//  Created by jokerx on 2025/11/10.
//

#include "exercise.h"
//void test();

void run_exercise(void) {
    test_string();
}

void test_string(void) {
    char str[] = "hello";
    // 下标访问到字符串实际长度的后一位,hello总共5位，看起来是0-4，下面循环访问了0-5，并未出现异常字符
    // 证实了字符串末尾自动加了一位空字符\0
    for (int i = 0; i < 5; i++) {
        printf("%c\n", str[i]);
    }
    // 验证空字符可否被打印 结果显示无法打印
    printf("print NUL=%c\n", '\0');
    // 试验手动给字符串末尾加\0的效果
    char str_with_nul[] = "halo\0";
    // 可见正常打印，现在疑问是，这样写末尾有几个NUL
    printf("str_with_nul=%s\n", str_with_nul);
    // 来一个同样字符但是不手动加\0的字符串
    char str_normal[] = "halo";
    // 同样文字，下标访问到5,也就是第6个元素
    // 手动添加NUL后，只是末尾有两个NUL，正常访问
    for (int i = 0; i < 6; i++) {
        printf("str_with_nul[%d]=%c\n", i, str_with_nul[i]);
    }
    // 未手动添加的，末尾只有一个NUL，访问越界时候只能找到一个随机内存存储内容
    for (int i = 0; i < 6; i++) {
        printf("str_normal[%d]=%c\n", i, str_normal[i]);
    }
    // 用字符数组方式写字符串，不主动加NUL，试验是否自动补NUL
    char manul_str[4] = {'h', 'a', 'l', 'o'};
    // 超出范围 全是NUL，暂未试验出结论，不知是自动补还是随机
    for (int i = 0; i < 9; i++) {
        printf("manul_str[%d]=%c\n", i, manul_str[i]);
    }
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
