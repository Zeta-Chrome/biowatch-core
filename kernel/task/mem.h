#ifndef KERNEL_MEM_H
#define KERNEL_MEM_H

#include "lib/status.h"
#include <stdint.h>

#define TASK_POOL_SIZE 0x4000 // 16kB

typedef uint32_t *stack_ptr_t;

enum mem_block_sz {
	MEM_BLOCK_SZ_128,
	MEM_BLOCK_SZ_256,
	MEM_BLOCK_SZ_512,
	MEM_BLOCK_SZ_1024,
	MEM_BLOCK_SZ_2048,
	MEM_BLOCK_COUNT
};

struct kernel_pool_conf {
	enum mem_block_sz sz;
	uint8_t count;
};

struct task_stack {
	stack_ptr_t stack_ptr;
	stack_ptr_t top_ptr;
	uint32_t size;
};

enum bw_status kernel_mem_init(const struct kernel_pool_conf *confs);
enum bw_status kernel_mem_alloc(struct task_stack *stack);
enum bw_status kernel_check_mem_sanity(struct task_stack *stack);
void kernel_mem_dealloc(struct task_stack *stack);

#endif
