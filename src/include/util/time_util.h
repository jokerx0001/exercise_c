#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#include <time.h>

/**
 * @brief 获取当前本地时间字符串（含毫秒）。
 *
 * 格式: "YYYY-MM-DD HH:MM:SS.mmm" 例如: 2025-11-29 14:03:25.123
 * 返回线程局部缓冲区指针；同一线程下一次调用会覆盖内容。
 * 失败时返回空字符串 ""。
 */
const char *util_get_time_str(void);

#endif // TIME_UTIL_H