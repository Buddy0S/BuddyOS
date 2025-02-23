    .section .text
    .global _start
_start:
    ldr sp, =_stack_top    /* Initialize sp through the definition from the linker */
    bl kernel_main         /* Call the kernel entry point */
    b .                    /* Infinite loop if kernel_main returns */

