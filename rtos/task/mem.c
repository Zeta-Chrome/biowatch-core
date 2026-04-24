#include "mem.h"
#include "cmsis_gcc.h"
#include "status.h"

#define NULL ((void *)0)
#define STACK_GUARD 0xA5A5A5A5

static uint64_t g_stack_pool[(TASK_POOL_SIZE >> 1) + 1] __attribute__((aligned(8)));
static stack_ptr_t g_free_block_heads[MEM_BLOCK_COUNT] = {NULL};

bw_status_t rtos_mem_init(const rtos_pool_conf_t *confs)
{
    uint32_t pool_idx = 0;
    for (int i = 0; confs[i].count > 0; i++)
    {
        uint16_t sz = 16 << confs[i].sz;  // each element is 8 bytes, hence 128/8
        uint8_t count = confs[i].count;

        g_free_block_heads[confs[i].sz] = (stack_ptr_t)&g_stack_pool[pool_idx + sz];
        for (int b = 0; b < count; b++)
        {
            if (b >= count - 1)
            {
                *(uint32_t*)(g_stack_pool + pool_idx + sz) = (uint32_t)NULL;
            }
            else
            {
                *(uint32_t*)(g_stack_pool + pool_idx + sz) = (uint32_t)&g_stack_pool[pool_idx + 2 * sz];
            }
            pool_idx = pool_idx + sz;
            if (pool_idx >= TASK_POOL_SIZE)
            {
                return STATUS_MEM_EXHAUSTED;
            }
        }
    }
    return STATUS_OK;
}

static inline uint8_t mem_block_sz(uint32_t word_sz)
{
    return __CLZ(__RBIT(word_sz * 4 / 128)) % 32;
}

bw_status_t rtos_mem_alloc(task_stack_t *stack)
{
    uint8_t block = mem_block_sz(stack->size);
    if (block >= MEM_BLOCK_COUNT || g_free_block_heads[block] == NULL)
    {
        return STATUS_MEM_EXHAUSTED;
    }

    stack->top_ptr = g_free_block_heads[block];
    stack->stack_ptr = stack->top_ptr;
    
    g_free_block_heads[block] = (stack_ptr_t)*(stack->top_ptr);
    *((uint32_t *)(stack->top_ptr - stack->size) + 1) = STACK_GUARD;

    return STATUS_OK;
}

bw_status_t rtos_check_mem_sanity(task_stack_t *stack)
{
    if ((uint32_t)(stack->top_ptr - stack->stack_ptr) > stack->size ||
        *((uint32_t *)(stack->top_ptr - stack->size) + 1) != STACK_GUARD)
    {
        return STATUS_STACK_OVR;
    }
    return STATUS_OK;
}

void rtos_mem_dealloc(task_stack_t *stack)
{
    mem_block_sz_t block = mem_block_sz(stack->size);
    *stack->top_ptr = (uint32_t)g_free_block_heads[block];
    g_free_block_heads[block] = stack->top_ptr;
}
