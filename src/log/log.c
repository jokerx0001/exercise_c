/**
 * @file log.c
 * @brief 日志模块实现文件，实现日志相关的函数
 * @details 实现日志写入接口和日志打印函数
 * @author JokerX
 * @date 2025-11-28
 */

#include "log/log.h"
#include "util/time_util.h"
#include <errno.h>
#include <pthread.h> // 新增：线程与同步
#include <stdbool.h>
#include <stdio.h>  // 新增：printf / FILE
#include <stdlib.h> // 新增：malloc/free
#include <string.h>
#include <unistd.h>

static const char *log_level_to_string(enum LOG_LEVEL level);

// 当前最小输出等级，外部可修改（未做线程保护）。
enum LOG_LEVEL current_log_level = DEBUG;
// 日志输出模板：时间 等级 消息\n
static const char *log_template = "[%s] [%s] %s\n";
static const char *file_path = "app.log";
// 日志文件可用标志
static bool is_file_enable = false;

// 文件句柄与后台写线程资源（模块内部持有）
static FILE *log_file = NULL;
static pthread_t worker_tid;
static pthread_mutex_t q_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t q_cond = PTHREAD_COND_INITIALIZER;
static bool worker_running = false;
static bool worker_exit = false;

// 有界队列（环形缓冲）
#define LOG_QUEUE_CAP 1024
#define LOG_MSG_MAX 1024
static char *log_queue[LOG_QUEUE_CAP];
static size_t q_head = 0, q_tail = 0, q_size = 0;

/**
 * @brief 将日志消息入队
 * @param msg 日志消息字符串
 */
static bool queue_push(const char *msg) {
  // 1) 队列满则直接丢弃（非阻塞），避免卡住生产者线程
  if (q_size == LOG_QUEUE_CAP)
    return false;

  // 2) 复制字符串入队
  // 最多拷贝LOG_MSG_MAX-1个字符.strnlen返回的是“到终止符之前的长度”，所以不会包括\0'
  size_t len = strnlen(msg, LOG_MSG_MAX - 1);
  // 空字符串 不处理 提前返回
  if (len == 0) {
    return false;
  }

  char *copy = malloc(len + 1);
  if (!copy)
    return false;
  memcpy(copy, msg, len);
  copy[len] = '\0';

  log_queue[q_tail] = copy;
  q_tail = (q_tail + 1) % LOG_QUEUE_CAP;
  q_size++;
  return true;
}

static char *queue_pop() {
  if (q_size == 0)
    return NULL;
  char *msg = log_queue[q_head];
  q_head = (q_head + 1) % LOG_QUEUE_CAP;
  q_size--;
  return msg;
}

// 后台写线程函数：消费队列写入文件
static void *log_worker(void *arg) {
  (void)arg;
  pthread_mutex_lock(&q_mutex);
  while (!worker_exit) {
    while (q_size == 0 && !worker_exit) {
      pthread_cond_wait(&q_cond, &q_mutex);
    }
    // 批量消费
    while (q_size > 0) {
      char *msg = queue_pop();
      pthread_mutex_unlock(&q_mutex);
      if (msg && log_file) {
        // 写入文件
        fputs(msg, log_file);
      }
      if (msg)
        free(msg);
      pthread_mutex_lock(&q_mutex);
    }
    // 刷盘
    if (log_file)
      fflush(log_file);
  }
  pthread_mutex_unlock(&q_mutex);
  return NULL;
}

// 核心输出函数：进行等级过滤 + 时间戳 + 格式化。
// 同步输出控制台，异步入队写文件（仅模块线程写文件）
static void write_log(const char *msg, enum LOG_LEVEL level) {
  // 过滤低等级日志
  if (level < current_log_level)
    return;

  const char *time_str = util_get_time_str();
  const char *level_str = log_level_to_string(level);
  char buf[LOG_MSG_MAX];

  // 格式化日志字符串并存入缓冲变量
  snprintf(buf, sizeof(buf), log_template, time_str, level_str, msg);
  // 控制台同步输出
  printf("%s", buf);

  // 文件异步写：拼装标准化字符串入队
  if (is_file_enable && worker_running) {
    pthread_mutex_lock(&q_mutex);
    bool ok = queue_push(buf);
    if (ok) {
      pthread_cond_signal(&q_cond);
    } else {
      // 队列满时，输出告警到stderr但不阻塞
      fprintf(stderr, "日志队列已满，丢弃一条消息\n");
    }
    pthread_mutex_unlock(&q_mutex);
  }
}

// 改进后的文件初始化：严格检查存在与写权限，打开文件句柄
static int log_file_init() {
  // 先判断文件是否存在
  if (access(file_path, F_OK) == 0) {
    // 文件存在，检查写权限
    if (access(file_path, W_OK) != 0) {
      fprintf(stderr, "日志文件 '%s' 无写权限: %s\n", file_path,
              strerror(errno));
      return -1;
    }
    // 以追加模式打开
    log_file = fopen(file_path, "a");
    if (log_file == NULL) {
      fprintf(stderr, "无法打开日志文件 '%s' 进行写入: %s\n", file_path,
              strerror(errno));
      return -1;
    }
  } else {
    // 文件不存在，尝试创建
    log_file = fopen(file_path, "w");
    if (log_file == NULL) {
      fprintf(stderr, "无法创建日志文件 '%s': %s\n", file_path,
              strerror(errno));
      return -1;
    }
  }
  is_file_enable = true;
  return 0;
}

// 新增：模块初始化（供外部在程序启动时调用）
int log_init(const char *path, enum LOG_LEVEL min_level) {
  if (path && *path)
    file_path = path;
  current_log_level = min_level;

  if (log_file_init() != 0) {
    // 文件不可用时，仅启用控制台输出
    is_file_enable = false;
    log_file = NULL;
    return -1;
  }

  worker_exit = false;
  if (pthread_create(&worker_tid, NULL, log_worker, NULL) != 0) {
    fprintf(stderr, "日志后台线程创建失败: %s\n", strerror(errno));
    // 关闭文件回退到控制台
    fclose(log_file);
    log_file = NULL;
    is_file_enable = false;
    return -1;
  }
  worker_running = true;
  return 0;
}

// 新增：模块关闭（在程序退出时调用）
void log_shutdown() {
  if (!worker_running)
    return;
  pthread_mutex_lock(&q_mutex);
  worker_exit = true;
  pthread_cond_signal(&q_cond);
  pthread_mutex_unlock(&q_mutex);

  pthread_join(worker_tid, NULL);
  worker_running = false;

  // 清空残留队列
  pthread_mutex_lock(&q_mutex);
  while (q_size > 0) {
    char *msg = queue_pop();
    if (msg)
      free(msg);
  }
  pthread_mutex_unlock(&q_mutex);

  if (log_file) {
    fflush(log_file);
    fclose(log_file);
    log_file = NULL;
  }
  is_file_enable = false;
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
