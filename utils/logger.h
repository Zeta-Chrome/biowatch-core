#ifndef PLATFORM_LOGGER_H
#define PLATFORM_LOGGER_H

#include <stdint.h>

#ifdef DEBUG
void bw_logger_init();
void bw_log(const char *file, int line, const char *fmt, ...);
#define BW_LOG(fmt, ...) bw_log(__FILE__, __LINE__, fmt, ##__VA_ARGS__);
#else
#define bw_logger_init() ((void)0)
#define BW_LOG(fmt, ...) ((void)0)
#endif

#endif
