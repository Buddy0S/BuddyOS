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

.extern supervisor_call

init:

   mrs r0, cpsr
   bic r0, r0, #0x1F @ clear mode bits
   orr r0, r0, #0x13 @ set svc mode
   msr cpsr, r0

   @ load vector table into vector table base address register
   @ our vector table is loaded at the start of sram
   @ Cortex a8 manual 3.2.68
   ldr r0, =vector_table
   mcr p15, #0, r0, c12, c0, #0

   @ set stack pointer for SVC mode and dsiable interrupts
   mrs r0, cpsr
   bic r0, r0, #0x1F @ clear mode bits
   orr r0, r0, #0x1F @ set System mode
   msr cpsr, r0

   ldr sp, =#0x8F000000 @ end of reserved

   @ jump to main
   bl main

   b .

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

.global switch_context
.type switch_context, %function
switch_context:
   /* OFFSETS:
    *   sp              28
    *   context         32
    *   end of context  64
    */
    
    add r2, r0, #64                 /* Get pointer to context */
    stmdb r2!, {r4-r11, lr}         /* Save registers and LR on current stack */
    str sp, [r0, #28]               /* Store current SP into *old_sp */
    ldr sp, [r1, #28]               /* Load new SP from *new_sp */
    add r3, r1, #64                 /* Gets pointer to context */
    ldmia r3!, {r4-r11, lr}         /* Restore registers and LR from new process stack */

    bx lr                           /* Branch to new process (thru restored LR) */
