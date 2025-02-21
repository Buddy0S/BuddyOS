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

init:

   @ load vector table into vector table base address register
   @ our vector table is loaded at the start of sram
   @ Cortex a8 manual 3.2.68
   @ldr r0, =__vector_load__
   @mcr p15, #0, r0, c12, c0, #0

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

    
