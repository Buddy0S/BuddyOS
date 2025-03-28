@ IRQ mode 0x12
@ FIQ mode 0x11
@ SVC mode 0x13

.global init
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

   @ switch into system mode
   mrs r0, cpsr
   bic r0, r0, #0x1F @ clear mode bits
   orr r0, r0, #0x1F @ set System mode
   msr cpsr, r0

   @ set stack pointer for system mode and dsiable interrupts
   cpsid if
   ldr sp, =#0x8F000000 @ end of reserved

   bl new_timer_init

   @ jump to main
   bl main

   b .

