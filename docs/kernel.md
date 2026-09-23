# Kernel

`kernel/` is a small preemptive, priority-based RTOS written from scratch for the Cortex-M4, with a tickless idle path designed around the LPM subsystem (see [lpm.md](lpm.md)).

## Task model

- Up to `MAX_TASKS` (32) tasks, `MAX_TASK_PRIORITY` (0–15) priority levels.
- Fixed-size TCB pool, no dynamic TCB allocation — `struct tcb` lives in a static pool; stacks come from the block-pool allocator in `kernel/task/mem.c`.
- Task states: `TASK_STATE_FREE`, `READY`, `BLOCKED`, `SUSPENDED`, tracked via intrusive list nodes (`state_node`, `delay_node`) rather than per-state arrays — moving a task between the ready queue, a delay queue, or a wait queue is an O(1) list splice.
- Context switch is implemented in `kernel/task/task_port.s` (PendSV-based) and completes in under 100 cycles.

```c
void kernel_task_create(task_func_t task_ptr, const char *name, uint8_t priority,
                         uint32_t stack_depth, void *p_user_data, task_handle_t *handle);
```

### Task notifications

Every task has a single built-in 32-bit notification value — a lightweight, allocation-free alternative to a semaphore for the common "one producer wakes one task" case (used to signal sensor-ready interrupts up to a task without a separate semaphore object per task):

```c
enum bw_status kernel_task_notify_wait(uint32_t clear_entry_mask, uint32_t clear_exit_mask,
                                        uint32_t *p_notification, uint64_t timeout_ms);
void kernel_task_notify(task_handle_t handle, uint32_t value, enum notify_action action);
void kernel_task_notify_from_isr(task_handle_t handle, uint32_t value, enum notify_action action);
```

`enum notify_action` supports `SET_BITS`, `INCREMENT`, and `OVERWRITE`, so the same primitive can act as a binary event flag, a counting signal, or a mailbox slot depending on what the caller needs. An ISR-safe variant (`_from_isr`) exists for every operation that can be called from interrupt context.

## Synchronization primitives (`kernel/sync/`)

| Primitive | File | Notes |
|---|---|---|
| Mutex | `mutex.c/h` | Supports recursive locking (owner + nest count tracked in the mutex struct) so a task can re-enter a lock it already holds without deadlocking itself. |
| Semaphore | `semaphore.c/h` | Counting semaphore, ISR-safe give. |
| Event | `event.c/h` | Bitmask event flags; a task can wait on "any" or "all" of a set of bits (`event_wait_all`/`event_clear_exit` in the TCB), used for multi-source synchronization (e.g. a task waiting on either a sensor IRQ or a timeout event). |
| Message queue | `mqueue.c/h` | Fixed-size, fixed-slot FIFO for passing structured records between tasks/ISRs (e.g. IMU samples from the interrupt handler to `task_act`). |

All four primitives are used throughout the application layer — e.g. the BLE task guards the GATT database with a mutex, sensor ISRs post to semaphores/mqueues that wake the corresponding measurement task, and the UI task waits on an event set combining button IRQs and the display timeout timer.

## Timers (`kernel/timer.c`)

Software timers (one-shot and periodic) driven off the same tick source as the scheduler:

```c
void     kernel_timer_start(struct kernel_timer *timer);
void     kernel_timer_stop(struct kernel_timer *timer);
uint64_t kernel_timer_deadline_ms();   // next timer expiry — consumed by the idle task to size the sleep window
uint64_t kernel_timer_ms();            // monotonic kernel time
```

`kernel_timer_deadline_ms()` is the link between the software-timer list and tickless idle: the idle task asks the timer subsystem "how long until something needs to happen" before deciding which low-power mode it can safely enter.

## Memory (`kernel/task/mem.c`)

Static block-pool allocator (`MEM_BLOCK_COUNT` pools configured in `struct kernel_conf`) used for TCB stacks. No `malloc`/heap is used for kernel objects — pool sizes and counts are fixed at `kernel_init()` time, so allocation is deterministic and there's no fragmentation risk on a memory-constrained MCU.

## Critical sections (`kernel/critical.c`)

Nestable interrupt masking (`critical_enter()`/`critical_exit()` style, with a nesting counter) used internally by the sync primitives and by drivers that need to protect a short register sequence from the scheduler or from a higher-priority ISR.

## Boot sequence

```c
kernel_init(&conf);   // sets up TCB/stack pools, idle task, idle hook
kernel_start();       // starts SysTick/LPTIM tick source, launches the scheduler — never returns
```

`struct kernel_conf.idle_hook` is where the LPM subsystem's sleep-selection logic is plugged in — the idle task calls it every time no other task is ready to run.
