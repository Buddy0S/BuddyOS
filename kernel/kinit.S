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

.extern testprint

.global switch_to_dispatch
.type switch_to_dispatch, %function
switch_to_dispatch:
   /* OFFSETS:
    *   sp              28
    *   context         32
    *   end of context  64
    */
    
    add r2, r0, #68                 /* Get pointer to context */
    stmdb r2!, {r4-r11, lr}         /* Save registers and LR on current stack */
    str sp, [r0, #28]               /* Store current SP into *old_sp */

    /* switch into system mode */
    mrs r2, cpsr
    bic r2, #0x1F
    orr r2, #0x1F
    msr cpsr, r2
    /*                         */
    cpsid if

    str sp, [r0, #68]               /* store user process sp */
    str lr, [r0, #72]               /* store user process lr */

    ldr sp, [r1, #28]               /* Load new SP from *new_sp */
    add r3, r1, #32                 /* Gets pointer to context */
    ldmia r3!, {r4-r11, lr}         /* Restore registers and LR from new process stack */

    bx lr                           /* Branch to new process (thru restored LR) */

.global switch_to_svc
.type switch_to_svc, %function
switch_to_svc:
   /* OFFSETS:
    *   sp              28
    *   context         32
    *   end of context  64
    */
    

    add r2, r0, #68                 /* Get pointer to context */
    stmdb r2!, {r4-r11, lr}         /* Save registers and LR on current stack */
    str sp, [r0, #28]               /* Store current SP into *old_sp */

    ldr sp, [r1, #68]               /* grab user process sp */
    ldr lr, [r1, #72]               /* grab user process sp */

    /* switch into svc mode */
    mrs r0, cpsr
    bic r0, #0x1F
    orr r0, #0x13
    msr cpsr, r0
    /*                         */

    ldr sp, [r1, #28]               /* Load new SP from *new_sp */
    add r3, r1, #32                 /* Gets pointer to context */
    ldmia r3!, {r4-r11, lr}         /* Restore registers and LR from new process stack */

    bx lr                           /* Branch to new process (thru restored LR) */

.global switch_to_start
.type switch_to_start, %function
switch_to_start:
   /* OFFSETS:
    *   sp              28
    *   context         32
    *   end of context  64
    */
    
    add r2, r0, #68                 /* Get pointer to context */
    stmdb r2!, {r4-r11, lr}         /* Save registers and LR on current stack */
    str sp, [r0, #28]               /* Store current SP into *old_sp */

    cpsie if
    /* switch into system mode */
    mrs r0, cpsr
    bic r0, #0x1F
    orr r0, #0x10
    msr cpsr, r0
    /*                         */

    ldr sp, [r1, #28]               /* Load new SP from *new_sp */
    add r3, r1, #32                 /* Gets pointer to context */
    ldmia r3!, {r4-r11, lr}         /* Restore registers and LR from new process stack */

    bx lr                           /* Branch to new process (thru restored LR) */
