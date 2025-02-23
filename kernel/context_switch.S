    .text
    .global switch_context
    .type switch_context, %function
switch_context:
    stmdb sp!, {r4-r11, lr}   /* Save registers and LR on current stack */
    str sp, [r0]              /* Store current SP into *old_sp */
    ldr sp, [r1]              /* Load new SP from *new_sp */
    ldmia sp!, {r4-r11, lr}   /* Restore registers and LR from new process stack */
    bx lr                     /* Branch to new process (thru restored LR) */

