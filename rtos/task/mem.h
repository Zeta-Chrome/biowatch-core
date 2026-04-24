#ifndef RTOS_MEM_H
#define RTOS_MEM_H

#include <stdint.h>
#include "utils/status.h"

#define TASK_POOL_SIZE 0x4000  // 16kB

typedef uint32_t *stack_ptr_t;

typedef enum
{
    MEM_BLOCK_SZ_128,
    MEM_BLOCK_SZ_256,
    MEM_BLOCK_SZ_512,
    MEM_BLOCK_SZ_1024,
    MEM_BLOCK_SZ_2048,
    MEM_BLOCK_COUNT
} mem_block_sz_t;

typedef struct
{
    mem_block_sz_t sz;
    uint8_t count;
} rtos_pool_conf_t;

typedef struct
{
    stack_ptr_t stack_ptr;
    stack_ptr_t top_ptr;
    uint32_t size;
} task_stack_t;

bw_status_t rtos_mem_init(const rtos_pool_conf_t *confs);
bw_status_t rtos_mem_alloc(task_stack_t *stack);
bw_status_t rtos_check_mem_sanity(task_stack_t *stack); 
void rtos_mem_dealloc(task_stack_t *stack);

#endif
