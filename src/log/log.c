/**
 * @file log.c
 * @brief 日志模块实现文件，实现日志相关的函数
 * @details 实现日志写入接口和日志打印函数
 * @author JokerX
 * @date 2025-11-28
 */

#include "log/log.h"
#include "util/time_util.h"

static const char *log_level_to_string(enum LOG_LEVEL level);

// 当前最小输出等级，外部可修改（未做线程保护）。
enum LOG_LEVEL current_log_level = DEBUG;
// 日志输出模板：时间 等级 消息\n
static const char *log_template = "[%s] [%s] %s\n";

// 核心输出函数：进行等级过滤 + 时间戳 + 格式化。
static void write_log(const char *msg, enum LOG_LEVEL level) {
  if (level < current_log_level)
    return; // 过滤低等级
  const char *time_str = util_get_time_str();
  const char *level_str = log_level_to_string(level);
  printf(log_template, time_str, level_str, msg);
}

static const char *log_level_to_string(enum LOG_LEVEL level) {
  switch (level) {
  case DEBUG:
    return "DEBUG";
  case INFO:
    return "INFO";
  case WARN:
    return "WARN";
  case ERROR:
    return "ERROR";
  default:
    return "UNKNOWN";
  }
}

void log_debug(const char *msg) { write_log(msg, DEBUG); }
void log_info(const char *msg) { write_log(msg, INFO); }
void log_warn(const char *msg) { write_log(msg, WARN); }
void log_error(const char *msg) { write_log(msg, ERROR); }
