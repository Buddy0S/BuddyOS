.section .text
.global _start

_start:
    /* Load stack pointer */
    ldr     sp, =_stack_top

    /* Call the C kernel main function */
    bl      kernel_main

