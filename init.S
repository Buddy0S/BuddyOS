
.global init

.extern __stack_start__

init:
   mrs r0, cpsr
   bic r0, r0, #0x1F @ clear mode bits
   orr r0, r0, #0x13 @ set Supervisor mode
   orr r0, r0, #0xC0 @ disable IRQ
   msr cpsr, r0

   ldr sp, =__stack_start__

   bl main

