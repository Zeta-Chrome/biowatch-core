#ifndef RTOS_RTOS_H
#define RTOS_RTOS_H

#include "rtos/task/mem.h"
#include "rtos/task/task.h"
#include "rtos/sync/semaphore.h"
#include "rtos/sync/mqueue.h"
#include "rtos/sync/mutex.h"
#include "rtos/sync/event.h"

typedef struct 
{
    rtos_pool_conf_t pool_confs[MEM_BLOCK_COUNT];
} rtos_conf_t;

void rtos_init(rtos_conf_t *conf);
void rtos_run();

#endif
