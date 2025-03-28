/* procedures that switch from one mode/context to another */

/*  PCB struct offsets:
 *      sp              28
 *      context         32
 *      top of context  68
 *      banked sp       68
 *      banked lr       72
 *      exception fp    76
 */

.global switch_to_dispatch
.type switch_to_dispatch, %function
switch_to_dispatch:
    
    add r2, r0, #68                 /* Get address of context */
    stmdb r2!, {r4-r11, lr}         /* Save registers and LR on current stack */
    str sp, [r0, #28]               /* Store current SP into *old_sp */

    /* switch into system mode */
    mrs r2, cpsr
    bic r2, #0x1F
    orr r2, #0x1F
    orr r2, #0xC0
    msr cpsr, r2
    /*                         */
    
    /* at this point we are in system mode, but we have the same sp and lr as
     * when we were in user mode, so we need to save those now before we load
     * the kernel sp so that we can restore these later */
    str sp, [r0, #68]               /* store user process sp */
    str lr, [r0, #72]               /* store user process lr */

    ldr sp, [r1, #28]               /* Load new SP from *new_sp */
    add r3, r1, #32                 /* Gets pointer to context */
    ldmia r3!, {r4-r11, lr}         /* Restore registers and LR from new process stack */

    bx lr                           /* Branch to new process (thru restored LR) */

.global switch_to_irq
.type switch_to_irq, %function
switch_to_irq:
    

    add r2, r0, #68                 /* Get address of context */
    stmdb r2!, {r4-r11, lr}         /* Save registers and LR on current stack */
    str sp, [r0, #28]               /* Store current SP into *old_sp */

    /* before we switch into irq mode, we need to load the correct user sp and
     * user lr before we switch to irq mode and place it into the shared banked
     * user/system registers so that later when we mode switch into user mode,
     * the correct sp and lr are already banked and will immediately be available */
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
    

    add r2, r0, #68                 /* Get address of context */
    stmdb r2!, {r4-r11, lr}         /* Save registers and LR on current stack */
    str sp, [r0, #28]               /* Store current SP into *old_sp */

    /* see switch_to_irq */
    ldr sp, [r1, #68]               /* grab user process sp */
    ldr lr, [r1, #72]               /* grab user process sp */

    /* switch into svc mode */
    mrs r0, cpsr
    bic r0, #0x1F
    orr r0, #0x13
    msr cpsr, r0
    /*                      */

    ldr sp, [r1, #28]               /* Load new SP from *new_sp */
    add r3, r1, #32                 /* Gets pointer to context */
    ldmia r3!, {r4-r11, lr}         /* Restore registers and LR from new process stack */

    bx lr                           /* Branch to new process (thru restored LR) */

.global switch_to_start
.type switch_to_start, %function
switch_to_start:

    /* no need to load user sp/lr ahead of time because we are switching
     * the user/system registers within the function without going through
     * a mode that has a different set of banked registers */
    
    add r2, r0, #68                 /* Get address of context */
    stmdb r2!, {r4-r11, lr}         /* Save registers and LR on current stack */
    str sp, [r0, #28]               /* Store current SP into *old_sp */


    cpsie if                        /* enable interrupts for user process */

    /* this might cause the timer irq to instantly go off and be serviced, but
     * the current process cpu_time is at the max, and so the irq handler will
     * either just come back here or the timer interrupt is so infrequent that
     * it doesn't really matter, BUT we should make sure that the min cpu_time
     * given to any process is at least two, so that we don't get an instant
     * timer irq that would change the scheduling while we are inside of this
     * procedure. it might work but i am not confident so better to avoid it */

    /* switch into user mode */
    mrs r0, cpsr
    bic r0, #0x1F
    orr r0, #0x10
    msr cpsr, r0
    /*                       */

    ldr sp, [r1, #28]               /* Load new SP from *new_sp */
    add r3, r1, #32                 /* Gets pointer to context */
    ldmia r3!, {r0, r5-r11, lr}     /* Restore registers and LR from new process stack */

    b proc_wrapper                  /* Branch to wrapper function */

