#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H

int bw_str_format(char *msg, int msg_len, const char *fmt, ...);

#ifdef DEBUG
void bw_logger_init();
void bw_print_s(const char *msg, int msg_len);
void bw_print(const char *fmt, ...);
void bw_log(const char *file, int line, const char *fmt, ...);
void bw_logs_flush();

#define BW_PRINT(fmt, ...) bw_print(fmt, ##__VA_ARGS__)
#define BW_LOG(fmt, ...) bw_log(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define BW_FLUSH_LOGS() bw_logs_flush()
#else
#define bw_logger_init() ((void)0)
#define bw_print_s(...) ((void)0)
#define BW_LOG(...) ((void)0)
#define BW_PRINT(...) ((void)0)
#define BW_FLUSH_LOGS() ((void)0)
#endif

#endif
