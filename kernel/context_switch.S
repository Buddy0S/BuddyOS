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
    orr r2, #0xC0
    msr cpsr, r2
    /*                         */

    str sp, [r0, #68]               /* store user process sp */
    str lr, [r0, #72]               /* store user process lr */

    ldr sp, [r1, #28]               /* Load new SP from *new_sp */
    add r3, r1, #32                 /* Gets pointer to context */
    ldmia r3!, {r4-r11, lr}         /* Restore registers and LR from new process stack */

    bx lr                           /* Branch to new process (thru restored LR) */

.global switch_to_irq
.type switch_to_irq, %function
switch_to_irq:
   /* OFFSETS:
    *   sp              28
    *   context         32
    *   end of context  64
    */
    

    add r2, r0, #68                 /* Get pointer to context */
    stmdb r2!, {r4-r11, lr}         /* Save registers and LR on current stack */
    str sp, [r0, #28]               /* Store current SP into *old_sp */

    ldr sp, [r1, #68]               /* grab user process sp */
    ldr lr, [r1, #72]               /* grab user process lr */

    /* switch into irq mode */
    mrs r0, cpsr
    bic r0, #0x1F
    orr r0, #0x12
    msr cpsr, r0
    /*                      */

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

    cpsie if                        /* enable interrupts for user process */
    /* switch into user mode */
    mrs r0, cpsr
    bic r0, #0x1F
    orr r0, #0x10
    msr cpsr, r0
    /*                         */

    ldr sp, [r1, #28]               /* Load new SP from *new_sp */
    add r3, r1, #32                 /* Gets pointer to context */
    ldmia r3!, {r4-r11, lr}         /* Restore registers and LR from new process stack */

    bx lr                           /* Branch to new process (thru restored LR) */
