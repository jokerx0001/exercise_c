// time_util.c
#include "util/time_util.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define TLS _Thread_local
#else
#if defined(__GNUC__) || defined(__clang__)
#define TLS __thread
#else
#define TLS
#endif
#endif

const char *util_get_time_str(void) {
  // 目标格式: YYYY-MM-DD HH:MM:SS.mmm => 23字符 + '\0' -> 24
  TLS static char buf[24];
  struct timeval tv;
  if (gettimeofday(&tv, NULL) != 0) {
    buf[0] = '\0';
    return buf;
  }
  struct tm tm;
#if defined(__APPLE__) || defined(__unix__)
  localtime_r(&tv.tv_sec, &tm);
#else
  struct tm *ptm = localtime(&tv.tv_sec);
  if (ptm == NULL) {
    buf[0] = '\0';
    return buf;
  }
  tm = *ptm;
#endif
  if (strftime(buf, 20, "%Y-%m-%d %H:%M:%S", &tm) == 0) { // 前19字符
    buf[0] = '\0';
    return buf;
  }
  long ms = tv.tv_usec / 1000; // 微秒转毫秒
  // 追加 .mmm
  snprintf(buf + 19, sizeof(buf) - 19, ".%03ld", ms);
  return buf;
}
