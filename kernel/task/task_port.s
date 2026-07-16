.syntax unified
.cpu cortex-m4
.fpu fpv4-sp-d16
.thumb

.section .text

.extern g_current_task
.extern task_scheduler  
.extern task_exit

.equ TCB_SP_OFFSET, 0

.thumb_func
.global task_stack_init
task_stack_init:
    /* r0 = &sp, r1 = pc, r2 = user_data */
    ldr r3, [r0] 
    push {r4}
    mov r4, #0x01000000 // Set xPSR thumb bit = 1
    str r4, [r3, #-4]!
    str r1, [r3, #-4]!
    ldr r4, =task_exit // Task returns to task_exit  
    str r4, [r3, #-4]! 
    sub r3, r3, #16
    str r2, [r3, #-4]! 
    sub r3, r3, #32
    ldr r1, =0xFFFFFFFD
    str r1, [r3, #-4]! // Push the exec return 
    str r3, [r0, #TCB_SP_OFFSET] // Save the stack pointer 
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
    cbz r2, next_task_handler

    mrs r3, psp // Get g_current_task sp
    tst lr, #0x10
    it eq
    vstmdbeq r3!, {s16-s31}
    stmdb r3!, {r4-r11}
    str lr, [r3, #-4]! 
    str r3, [r2, #TCB_SP_OFFSET] // Save g_current_task sp

next_task_handler:
    ldr r3, [r0, #TCB_SP_OFFSET] // Get g_next_task sp
    ldr lr, [r3], #4
    ldmia r3!, {r4-r11}
    tst lr, #0x10
    it eq
    vldmiaeq r3!, {s16-s31}
    msr psp, r3 // Load g_next_task sp

    str r0, [r1] // g_current_task = g_next_task
        
exit_handler:
    dsb
    isb
    bx lr
