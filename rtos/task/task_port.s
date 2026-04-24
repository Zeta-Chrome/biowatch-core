.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.section .text

.extern g_current_task
.extern task_scheduler  
.extern task_exit

.thumb_func
.global task_stack_init
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
    push {lr}
    bl task_scheduler
    pop {lr}
    
    ldr r0, =g_next_task      
    ldr r0, [r0]
    cbz r0, exit_handler

    ldr r1, =g_current_task
    ldr r2, [r1]
    cbz r2, next_task

    mrs r3, psp // Get g_current_task sp
    stmdb r3!, {r4-r11}
    str r3, [r2] // Save g_current_task sp after push

next_task:
    ldr r3, [r0] // Get g_next_task sp
    ldmia r3!, {r4-r11}
    msr psp, r3 // Load g_next_task sp after push

    str r0, [r1] // g_current_task = g_next_task
    # ldr r1, =g_next_task      
    # mov r0, #0
    # str r0, [r1] // g_next_task = NULL
    
exit_handler:
    ldr lr, =0xFFFFFFFD
    bx lr
