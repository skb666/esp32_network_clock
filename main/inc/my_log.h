#ifndef __MY_LOG_H__
#define __MY_LOG_H__

#include <stdio.h>

/* Define trace levels */
#define MY_TRACE_LEVEL_NONE 0    /* No trace messages to be generated    */
#define MY_TRACE_LEVEL_ERROR 1   /* Error condition trace messages       */
#define MY_TRACE_LEVEL_WARNING 2 /* Warning condition trace messages     */
#define MY_TRACE_LEVEL_INFO 3    /* Debug messages for info            */
#define MY_TRACE_LEVEL_DEBUG 4   /* Full debug messages                  */
#define MY_TRACE_LEVEL_VERBOSE 5 /* Verbose debug messages               */

#define MAX_TRACE_LEVEL 6

#define DEFAULT_LOG_LEVEL MY_TRACE_LEVEL_INFO

#define LOG_COLOR_YELLOW "\033[1;33m"
#define LOG_COLOR_RED "\033[1;31m"
#define LOG_COLOR_BLUE "\033[1;34m"
#define LOG_COLOR_GREEN "\033[0;32m"
#define LOG_COLOR_PURPLE "\033[1;35m"
#define LOG_COLOR_RESET "\033[0m"

#define LOGCOMMON(tag, fmt, ...) printf(LOG_COLOR_BLUE "[ %s ] " LOG_COLOR_RESET fmt, tag, ##__VA_ARGS__);
#define LOGINFO(tag, fmt, ...) printf(LOG_COLOR_BLUE "[ %s ] " LOG_COLOR_RESET fmt "\n", tag, ##__VA_ARGS__);
#define LOGWARN(tag, fmt, ...) printf(LOG_COLOR_BLUE "[ %s ] " LOG_COLOR_RESET LOG_COLOR_YELLOW fmt LOG_COLOR_RESET "\n", tag, ##__VA_ARGS__);
#define LOGERR(tag, fmt, ...) printf(LOG_COLOR_BLUE "[ %s ] " LOG_COLOR_RESET LOG_COLOR_RED fmt LOG_COLOR_RESET "\n", tag, ##__VA_ARGS__);

#define MY_LOGE(tag, fmt, ...)                           \
    do {                                                 \
        if (DEFAULT_LOG_LEVEL >= MY_TRACE_LEVEL_ERROR) { \
            LOGERR(tag, fmt, ##__VA_ARGS__);             \
        }                                                \
    } while (0)
#define MY_LOGW(tag, fmt, ...)                             \
    do {                                                   \
        if (DEFAULT_LOG_LEVEL >= MY_TRACE_LEVEL_WARNING) { \
            LOGWARN(tag, fmt, ##__VA_ARGS__);              \
        }                                                  \
    } while (0)
#define MY_LOGI(tag, fmt, ...)                          \
    do {                                                \
        if (DEFAULT_LOG_LEVEL >= MY_TRACE_LEVEL_INFO) { \
            LOGINFO(tag, fmt, ##__VA_ARGS__);         \
        }                                               \
    } while (0)
#define MY_LOGC(tag, fmt, ...)                          \
    do {                                                \
        if (DEFAULT_LOG_LEVEL >= MY_TRACE_LEVEL_INFO) { \
            LOGCOMMON(tag, fmt, ##__VA_ARGS__);         \
        }                                               \
    } while (0)
#define MY_LOGD(tag, fmt, ...)                           \
    do {                                                 \
        if (DEFAULT_LOG_LEVEL >= MY_TRACE_LEVEL_DEBUG) { \
            LOGCOMMON(tag, fmt, ##__VA_ARGS__);          \
        }                                                \
    } while (0)
#define MY_LOGV(tag, fmt, ...)                             \
    do {                                                   \
        if (DEFAULT_LOG_LEVEL >= MY_TRACE_LEVEL_VERBOSE) { \
            LOGCOMMON(tag, fmt, ##__VA_ARGS__);            \
        }                                                  \
    } while (0)

#endif /* __MYLOG_H__ */
