/*
	CMPT432 - Implementation Team 00
*/

@ IRQ mode 0x12
@ FIQ mode 0x11
@ SVC mode 0x13

.global init
.global READ32
.global READ16
.global READ8
.global WRITE32
.global WRITE16
.global WRITE8

.extern __stack_SVC_start__
.extern __stack_IRQ_start__
.extern __stack_FRQ_start__
.extern __sram_start__
.extern __vector_load__

@section 2.14 cortex a8 manual
@cpsr current program status register 
@ first 5 bits are mode bits
@ bits 6 and 7 are FRQ and IRQ disable
init:

   @set stack for IRQ mode
   mrs r0, cpsr
   bic r0, r0, #0x1F @ clear mode bits
   orr r0, r0, #0x12 @ set IRQ mode
   orr r0, r0, #0xC0 @ disable interrupts?
   msr cpsr, r0

   ldr sp, =__stack_IRQ_start__

   @set stack for FRQ mode
   mrs r0, cpsr
   bic r0, r0, #0x1F @ clear mode bits
   orr r0, r0, #0x11 @ set FRQ mode
   msr cpsr, r0

   ldr sp, =__stack_FRQ_start__

   @ set stack pointer for SVC mode and dsiable interrupts
   mrs r0, cpsr
   bic r0, r0, #0x1F @ clear mode bits
   orr r0, r0, #0x13 @ set Supervisor mode
   orr r0, r0, #0xC0 @ disable IRQ and FRQ
   msr cpsr, r0

   ldr sp, =__stack_SVC_start__

   @ load vector table into vector table base address register
   @ our vector table is loaded at the start of sram
   @ Cortex a8 manual 3.2.68
   ldr r0, =__vector_load__
   mcr p15, #0, r0, c12, c0, #0
    
   @ jump to main
   bl main

WRITE32:
    str r1,[r0]
    bx lr

READ32:
    ldr r0,[r0]
    bx lr

WRITE16:
    strh r1,[r0]
    bx lr

READ16:
    ldrh r0,[r0]
    bx lr

WRITE8:
    strb r1,[r0]
    bx lr

READ8:
    ldrb r0,[r0]
    bx lr

    
