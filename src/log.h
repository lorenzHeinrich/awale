#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_ERROR 3

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

// a format string is always required, e.g. LOG_INFO("started") not LOG_INFO()
#define LOG(level, ...)                                                        \
  do {                                                                         \
    fprintf(stderr, "[%s] %s:%d: ", level, __FILE__, __LINE__);                \
    fprintf(stderr, __VA_ARGS__);                                              \
    fprintf(stderr, "\n");                                                     \
  } while (0)

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_DEBUG(...) LOG("DEBUG", __VA_ARGS__)
#else
#define LOG_DEBUG(...)                                                         \
  ((void)0) // compiles to nothing, arguments are never evaluated
#endif

#if LOG_LEVEL <= LOG_LEVEL_INFO
#define LOG_INFO(...) LOG("INFO", __VA_ARGS__)
#else
#define LOG_INFO(...) ((void)0)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
#define LOG_WARN(...) LOG("WARN", __VA_ARGS__)
#else
#define LOG_WARN(...) ((void)0)
#endif

#define LOG_ERROR(...) LOG("ERROR", __VA_ARGS__)

#endif
