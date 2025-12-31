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
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#if defined(__linux__)
#include <sys/syscall.h>
#endif

static const char *log_level_to_string(enum LOG_LEVEL level);

// 当前最小输出等级，外部可修改（未做线程保护）。
enum LOG_LEVEL current_log_level = DEBUG;
// 日志输出模板：时间 等级 [线程名] 消息\n
static const char *log_template = "%s %s [%s] %s\n";
static const char *file_path = "app.log";
// 日志文件可用标志
static bool is_file_enable = false;

// 文件句柄与后台写线程资源（模块内部持有）
static FILE *log_file = NULL;
static pthread_t worker_tid;
static pthread_mutex_t q_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t q_cond = PTHREAD_COND_INITIALIZER;
static bool worker_running = false;

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
  // +1：把尾指针往前移动一个槽位（刚入队占用了当前位置，下一次应写到下一个位置）。
  // % LOG_QUEUE_CAP：到达数组末尾时从头绕回，保证索引始终在 [0, CAP-1] 范围内。
  q_tail = (q_tail + 1) % LOG_QUEUE_CAP;
  q_size++;
  return true;
}

/**
 * @brief 从队列中弹出一条日志消息
 * @return 日志消息字符串指针，调用者负责释放；队列空时返回NULL
 */
static char *queue_pop() {
  if (q_size == 0)
    return NULL;
  char *msg = log_queue[q_head];
  q_head = (q_head + 1) % LOG_QUEUE_CAP;
  q_size--;
  return msg;
}

/**
 * @brief 日志写入后台线程函数
 * @param arg 未使用参数
 */
static void *log_worker(void *arg) {
  (void)arg;
  pthread_mutex_lock(&q_mutex);
  while (worker_running) {
    while (q_size == 0 && worker_running) {
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

/**
 * @brief 获取当前线程名称，若不可用则返回线程ID字符串
 * @param buf 输出缓冲区
 * @param size 缓冲区大小
 */
static void get_thread_name(char *buf, size_t size) {
  if (!buf || size == 0)
    return;
  buf[0] = '\0';
  pthread_t current_thread = pthread_self();
  // 尝试获取线程名
  if (pthread_getname_np(current_thread, buf, size) != 0 || buf[0] == '\0') {
    // 回退为线程ID字符串
#if defined(__APPLE__)
    uint64_t tid = 0;
    if (pthread_threadid_np(NULL, &tid) == 0) {
      snprintf(buf, size, "tid:%llu", (unsigned long long)tid);
    } else {
      snprintf(buf, size, "tid:%lu", (unsigned long)pthread_self());
    }
#elif defined(__linux__)
    long tid = syscall(SYS_gettid);
    snprintf(buf, size, "tid:%ld", tid);
#else
    snprintf(buf, size, "tid:%lu", (unsigned long)pthread_self());
#endif
  }
}

/**
 * @brief 核心日志写入函数
 * @param msg 日志消息
 * @param level 日志等级
 */
static void write_logf(enum LOG_LEVEL level, const char *fmt, va_list ap) {
  // 过滤低等级日志
  if (level < current_log_level)
    return;

  const char *time_str = util_get_time_str();
  const char *level_str = log_level_to_string(level);
  char buf[LOG_MSG_MAX];
  char thread_name[64];
  get_thread_name(thread_name, sizeof(thread_name));

  // 先格式化用户消息
  char msg[LOG_MSG_MAX];
  vsnprintf(msg, sizeof(msg), fmt, ap);
  // 再拼装模板
  snprintf(buf, sizeof(buf), log_template, time_str, level_str, thread_name,
           msg);
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

/**
 * @brief 初始化日志文件
 * @return 成功返回0，失败返回-1
 */
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

  worker_running = true;
  if (pthread_create(&worker_tid, NULL, log_worker, NULL) != 0) {
    fprintf(stderr, "日志后台线程创建失败: %s\n", strerror(errno));
    // 关闭文件回退到控制台
    fclose(log_file);
    log_file = NULL;
    is_file_enable = false;
    worker_running = false;
    return -1;
  }
  return 0;
}

void log_shutdown() {
  if (!worker_running)
    return;
  pthread_mutex_lock(&q_mutex);
  worker_running = false;
  pthread_cond_broadcast(&q_cond);
  pthread_mutex_unlock(&q_mutex);

  pthread_join(worker_tid, NULL);

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

/**
 * @brief 将日志等级枚举转换为字符串
 * @param level 日志等级枚举值
 * @return 对应的字符串表示
 */
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

void log_debug(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  write_logf(DEBUG, fmt, ap);
  va_end(ap);
}

void log_info(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  write_logf(INFO, fmt, ap);
  va_end(ap);
}

void log_warn(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  write_logf(WARN, fmt, ap);
  va_end(ap);
}

void log_error(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  write_logf(ERROR, fmt, ap);
  va_end(ap);
}
