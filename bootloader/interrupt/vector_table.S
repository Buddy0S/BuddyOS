/*
	CMPT432 - Implementation Team 00
*/

.global vector_table

@ ARM V7 Vector Table
@ https://developer.arm.com/documentation/den0013/d/Exception-Handling/Exception-priorities?lang=en#CEGHDCAE

.balign 32
vector_table:
    b init                   @ Used for Reset
    b undefined_instruction
    b temp_supervisor_call
    b prefetch_abort
    b data_abort
    b not_used
    b irq_interrupt
    b fiq_interrupt


@ when an exception happens it jumps to the label 
@ depending on what type of exception it is

undefined_instruction:
    ldr r0, =3
    b loop

temp_supervisor_call:

    ldr r0, =1
    b loop

prefetch_abort:
    ldr r0, =4
    b loop

data_abort:
    ldr r0, =2
    b loop

not_used:
    b loop

@ interrupt exception handler
irq_interrupt:
    
    @ save registers to stack
    stmfd sp!, {r0-r3, r11, lr}
 
    @ save the saved program status register
    mrs r11, spsr

    @ jump to interrupt handler 
    bl interrupt_handler

    @ TI manual 6.6.1.6
    @ acknowledge interrupt to interrupt controller
    ldr r0, =0x48200000 @ INTC base address
    mov r1, #0x1 @ gonna set bit one to enable new irq generation
    str r1, [r0, #0x48] @ write the bit to INTC_CONTROL register

    msr spsr, r11 @ restore saved program status register
    ldmfd sp!, {r0-r3, r11, lr} @ restore saved registers

    subs pc, lr, #4 @ restore execution


fiq_interrupt:
    b loop


loop:

    @ jump to exception handler
    bl exception_handler

    b loop
