//
//  exercise.c
//  exercise
//
//  Created by jokerx on 2025/11/10.
//

#include "exercise.h"
//void test();

void run_exercise(void) {
    test_param();
}

void exercise_struct(void) {
    struct Rectangle rect1;
    rect1.width = 1;
    rect1.height = 2;
    printf("rect1 h=%d\n", rect1.height);
    printf("rect1 w=%d\n", rect1.width);
    
    // 结构体也遵循入参规则，非指针类型传入函数也无法被改变
    // 可见结构体变量直接传入to_square后，虽然方法内部的结构体中某个参数被改变后是新的值
    // 一旦方法执行结束，结构体变量的值还是没有变化
    to_square(rect1);
    struct Rectangle *rectangle_p;
    rectangle_p = &rect1;
    to_square_p(rectangle_p);
    to_square_p(&rect1);
    
    struct Response resp;
//    strcpy(resp.msg, "hello");
    resp.msg = "hello";
    printf("resp msg=%s\n", resp.msg);
}

void test_param(void) {
    int i = 1;
    test_change_param(i);
    printf("i=%d\n", i);
}

void test_change_param(int i) {
    i = 5;
    printf("i=%d\n", i);
}

void to_square(struct Rectangle rect) {
    rect.height = rect.width;
    printf("rect formatted to square!w=%d, h=%d\n", rect.width, rect.height);
}

void to_square_p(struct Rectangle *rect) {
    rect->width = rect->height;
    printf("rect formatted to square!w=%d, h=%d\n", rect->width, rect->height);
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
    // 结论是即使手动加了\0,系统依然再加一个\0
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

void string_func(void) {
    char origin[] = "origin hello";
    char target_short[5];
    char target_equal[13];
    char target_longer[130];
    
    // 如果目标字符串长度比来源短，程序会异常
    //strcpy(target_short, origin);
    // 刚好相等，正常使用
    strcpy(target_equal, origin);
    printf("target_equal=%s\n", target_equal);
    // 目标更长，正常使用
    strcpy(target_longer, origin);
    printf("target_longer=%s\n", target_longer);
    // 尝试继续向长字符串拷贝内容
    char ori2[] = ".Today is rainy.";
    strcat(target_longer, ori2);
    printf("target_longer=%s\n", target_longer);
    // 数组 char s[125]; 声明后已经在栈上分配了 125 个字节的内存(若是全局/静态则在静态区)，只是内容未初始化，里面是未定义值。strlen 计算以首个 '\0' 结束的字符串长度，不是缓冲区容量。在你 strcpy(s, "hello"); 后，s 里是 h e l l o \0 ...，所以 strlen(s) 返回5
    printf("target_longer's size=%zu\n", strlen(target_longer));
    
    char str_cmp1[] = "5";
    char str_cmp2[] = "5";
    printf("str1=%s, str2=%s, cmp=%d\n", str_cmp1, str_cmp2, strcmp(str_cmp1, str_cmp2));
    char *p_str_cmp1 = "abcdef";
    char *p_str_cmp2 = "qwerrt";
    // strcmp比较第一个不相同字符的ascII值
    printf("str1=%s, str2=%s, cmp=%d\n", p_str_cmp1, p_str_cmp2, strcmp(p_str_cmp1, p_str_cmp2));
    
    char *fp = strchr("hellohello", 'l');
    fp = fp + 2;
    printf("fp=%c\n", *fp);
    
    char *str_fp = strstr("hellohello", "lo");
    printf("str_fp=%c\n", *str_fp);
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
