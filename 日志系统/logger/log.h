/**
 * 玄隐日志系统
 * 分级日志，支持彩色输出和文件记录
 */

#ifndef XUANYIN_LOG_H
#define XUANYIN_LOG_H

#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// 日志级别
typedef enum {
    LOG_LEVEL_TRACE = 0,    // 跟踪
    LOG_LEVEL_DEBUG,        // 调试
    LOG_LEVEL_INFO,         // 信息
    LOG_LEVEL_WARN,         // 警告
    LOG_LEVEL_ERROR,        // 错误
    LOG_LEVEL_FATAL,        // 致命
    LOG_LEVEL_COUNT
} LogLevel;

// 日志输出目标
typedef enum {
    LOG_TARGET_CONSOLE = 0x01,  // 控制台
    LOG_TARGET_FILE    = 0x02,   // 文件
    LOG_TARGET_ALL     = 0x03    // 全部
} LogTarget;

// 日志配置
typedef struct {
    LogLevel min_level;          // 最小日志级别
    int targets;                 // 输出目标
    const char* log_dir;         // 日志目录
    int max_file_size;           // 最大文件大小(MB)
    int max_file_count;          // 最大文件数量
    int enable_color;            // 启用颜色
    int enable_timestamp;        // 启用时间戳
} LogConfig;

// 日志初始化
int log_init(const LogConfig* config);

// 日志销毁
void log_destroy(void);

// 设置日志级别
void log_set_level(LogLevel level);

// 设置输出目标
void log_set_targets(int targets);

// 日志函数
void log_write(LogLevel level, const char* file, int line, 
               const char* func, const char* format, ...);

// 简化宏
#define LOG_TRACE(...) log_write(LOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_DEBUG(...) log_write(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_INFO(...)  log_write(LOG_LEVEL_INFO,  __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_WARN(...)  log_write(LOG_LEVEL_WARN,  __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_ERROR(...) log_write(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define LOG_FATAL(...) log_write(LOG_LEVEL_FATAL, __FILE__, __LINE__, __func__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // XUANYIN_LOG_H