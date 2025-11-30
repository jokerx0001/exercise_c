/**
 * @file log.h
 * @brief 日志模块头文件，定义日志相关的函数和宏，以及基本数据结构
 * @details 定义日志等级枚举。定义日志写入接口。
 * @author JokerX
 * @date 2025-11-28
 */
#ifndef log_h
#define log_h

#include <stdio.h>
#include <string.h>

/**
 * @brief 日志等级枚举
 * @details 定义不同的日志等级,只打印 >=min_level的日志
 */
enum LOG_LEVEL { DEBUG, INFO, WARN, ERROR };

/**
 * @brief 打印 DEBUG 等级日志。
 * @param msg 日志信息
 */
void log_debug(const char *msg);

/**
 * @brief 打印 INFO 等级日志。
 * @param msg 日志信息
 */
void log_info(const char *msg);

/**
 * @brief 打印 WARN 等级日志。
 * @param msg 日志信息
 */
void log_warn(const char *msg);

/**
 * @brief 打印 ERROR 等级日志。
 * @param msg 日志信息
 */
void log_error(const char *msg);

#endif /* log_h */
