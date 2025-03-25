
.global vector_table

@ ARM V7 Vector Table
@ https://developer.arm.com/documentation/den0013/d/Exception-Handling/Exception-priorities?lang=en#CEGHDCAE

.balign 32
vector_table:
    b init                   @ Used for Reset
    b undefined_instruction
    b supervisor_call
    b prefetch_abort
    b data_abort
    b not_used
    b irq_interrupt
    b fiq_interrupt


@ when an exception happens it jumps to the label 
@ depending on what type of exception it is

undefined_instruction:
    ldr r0, =3
    mov r1, lr
    b loop

prefetch_abort:
    ldr r0, =4
    mov r1, lr
    b loop

data_abort:
    ldr r0, =2
    mov r1, lr
    b loop

not_used:
    b loop

.extern current_process

@ interrupt exception handler
irq_interrupt:
    
    @ save registers to stack
    ldr sp, =current_process
    ldr sp, [sp]
    ldr sp, [sp, #76]
    stmfd sp!, {r0-r3, r11, lr}
 
    @ save the saved program status register
    mrs r11, spsr

    @ TI manual 6.6.1.6
    @ acknowledge interrupt to interrupt controller
    ldr r0, =0x48200000 @ INTC base address
    mov r1, #0x1 @ gonna set bit one to enable new irq generation
    str r1, [r0, #0x48] @ write the bit to INTC_CONTROL register

    mov r0, sp

    @ jump to interrupt handler 
    bl interrupt_handler

    msr spsr, r11 @ restore saved program status register
    ldmfd sp!, {r0-r3, r11, lr} @ restore saved registers

    subs pc, lr, #4 @ restore execution


fiq_interrupt:
    b loop

loop:

    @ jump to exception handler
    bl kexception_handler
    b loop

.extern testprint


@ can't use svc in supervisor node
.global supervisor_call
supervisor_call:

    @ save registers to stack
    ldr sp, =current_process
    ldr sp, [sp]
    ldr sp, [sp, #76]
    /* ????? lol ????? */

    stmfd sp!, {r0-r3, r11, lr}

    @ save address of stack for access to parameters
    mov r1, sp
 
    @ save the saved program status register
    mrs r11, spsr

    @ get svc number
    ldr r0, [lr, #-4]
    bic r0, #0xFF000000

    @ svc num in r0, parameters array in r1

    @ jump to interrupt handler 
    bl svc_handler

    msr spsr, r11 @ restore saved program status register
    ldmfd sp!, {r0-r3, r11, lr} @ restore saved registers

    subs pc, lr, #0 @ restore execution


