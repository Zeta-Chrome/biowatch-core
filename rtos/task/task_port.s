.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.section .text

.thumb_func
.global task_stack_init
.extern g_current_task
.extern task_scheduler  
.extern task_exit
task_stack_init:
    /* r0 = &sp, r1 = pc, r2 = user_data */
    ldr r3, [r0] 
    push {r4}
    mov r4, #0x01000000 // Set xPSR tumb bit = 1
    str r4, [r3, #-4]!
    str r1, [r3, #-4]!
    ldr r4, =task_exit // Task returns to task_exit  
    str r4, [r3, #-4]! 
    sub r3, r3, #48
    str r2, [r3, #-4]! 
    str r3, [r0] // Save the top pointer for the stack 
    pop {r4}
    bx lr

.thumb_func
.global PendSV_Handler 
PendSV_Handler:
    mrs r0, psp // Get the current task stack pointer
    cbz r0, switch_context // Skip if psp is 0 

    stmdb r0!, {r4-r11}
    ldr r1, =g_current_task     
    ldr r1, [r1]
    str r0, [r1]

switch_context:
    push {lr}
    bl task_scheduler
    pop {lr}

    ldr r0, =g_current_task      
    ldr r0, [r0] // r0 has the pointer to the tcb
    ldr r1, [r0] // r1 has the stack pointer
    
    ldmia r1!, {r4-r11}
    msr psp, r1

    ldr lr, =0xFFFFFFFD
    bx lr
