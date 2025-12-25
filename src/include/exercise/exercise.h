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

struct Rectangle {
  int width;
  int height;
};

struct Response {
  int code;
  char *msg;
};

union Data {
  int i;
  float j;
  char str[20];
};

void run_exercise(void);
void test_string(void);
void exercise_callback(void);
int callback(int a, int b, int (*p)(int, int));
int plus(int a, int b);
void string_func(void);
void exercise_struct(void);
void to_square(struct Rectangle rect);
void to_square_p(struct Rectangle *rect);
void test_param(void);
void test_change_param(int i);
void exercise_union(void);
void exercise_file(void);
void mirror_game(void);
void test_malloc(void);
void test_undified(void);

#endif /* exercise_h */
