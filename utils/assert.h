#ifndef PLATFORM_ASSERT_H
#define PLATFORM_ASSERT_H

#ifdef DEBUG

#include "utils/logger.h"
void bw_assert_handler();
#define BW_ASSERT(expr, msg, ...)                                                                            \
    do                                                                                                  \
    {                                                                                                   \
        if (!(expr))                                                                                    \
        {                                                                                               \
            bw_log(__FILE__, __LINE__, msg, ##__VA_ARGS__);                                             \
            bw_assert_handler();                                                                        \
        }                                                                                               \
    } while (0)
#else
#define BW_ASSERT(expr, msg, ...) ((void)0)
#endif

#endif
