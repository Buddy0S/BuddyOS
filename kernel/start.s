    .section .text
    .global _start
    .global context_switch
    .extern current_pcb
    .extern next_pcb
    .extern kernel_main

/* _start: Entry point */
_start:
    /* Set up the initial stack pointer */
    ldr sp, =_stack_top
    bl kernel_main

1:
    b 1b   /* Infinite loop if kernel_main returns */

/*
 * context_switch:
 *   - If a current process exists, save its registers onto its stack.
 *   - Load the stack pointer from next_pcb and update current_pcb.
 *   - Restore registers from the new process's saved context.
 *
 * We save/restore 14 registers: r0–r12 and lr.
 */
context_switch:
    ldr r0, = current_pcb
    ldr r1, [r0]         /* r1 = current_pcb */
    cmp r1, 
    beq load_next        /* If no current process, skip saving */

    push {r0-r12, lr}    /* Save registers onto current process stack */
    str sp, [r1]         /* Update current_pcb->stack_ptr */

load_next:
    ldr r0, =next_pcb
    ldr r1, [r0]         /* r1 = next_pcb */
    ldr sp, [r1]         /* Set SP to next_pcb->stack_ptr */
    ldr r0, = current_pcb
    str r1, [r0]         /* current_pcb = next_pcb */
    pop {r0-r12, lr}     /* Restore registers from new process's stack */
    bx lr

/* In the real system, wherever on the RAM where we allocated the stack
   is where this would be set to*/
    .global _stack_top
_stack_top:
    .word 0x80000000

