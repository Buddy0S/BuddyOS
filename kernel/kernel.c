#include "uart.h"
#include "proc.h"
#include "mm.h"
#include <stdint.h>

#define STACK_SIZE    4096
#define MAX_PROCESSES 1

/* Implement this later when real scheduling */
void yield(void);

/* Process stack for Process 1 (must be 8-byte aligned) */
uint8_t process1_stack[STACK_SIZE] __attribute__((aligned(8)));

/* Process table and global process pointers.
   We use 0 instead of NULL because we dont have NULL
  (maybe we could make our own?) */
PCB process_table[MAX_PROCESSES];
PCB *current_pcb = 0;
PCB *next_pcb = 0;

extern void context_switch(void);

/* This is a dummy process 1, not real but
   it prints a message and then loops forever. */
void process1_entry(void) {
    uart_puts("This is process 1, infinitely loop\n");
    while (1) {
        /* Process halts here */
    }
}

/*
 * Initialize a process control block (PCB) and its stack.
 *
 * We reserve space for 14 32-bit words arranged as follows:
 *   Word 0: r0    (0)
 *   Word 1: r1    (0)
 *   Word 2: r2    (0)
 *   Word 3: r3    (0)
 *   Word 4: r4    (0)
 *   Word 5: r5    (0)
 *   Word 6: r6    (0)
 *   Word 7: r7    (0)
 *   Word 8: r8    (0)
 *   Word 9: r9    (0)
 *   Word 10: r10  (0)
 *   Word 11: r11  (0)
 *   Word 12: r12  (0)
 *   Word 13: lr   (process entry point)
 *
 * When the context_switch routine later does:
 *    pop {r0-r12, lr}
 * then registers r0–r12 will be restored as 0 and lr will be set to the entry point,
 * causing a branch (thru "bx lr") into the process entry function.
 */
void init_process(PCB *pcb, void (*entry)(), uint8_t *stack) {
    pcb->stack_base = (uint32_t *)stack;
    pcb->stack_ptr = (uint32_t *)((uint8_t *)stack + STACK_SIZE - 14 * sizeof(uint32_t));

    uint32_t *sp = pcb->stack_ptr;
    sp[0]  = 0;              // r0
    sp[1]  = 0;              // r1
    sp[2]  = 0;              // r2
    sp[3]  = 0;              // r3
    sp[4]  = 0;              // r4
    sp[5]  = 0;              // r5
    sp[6]  = 0;              // r6
    sp[7]  = 0;              // r7
    sp[8]  = 0;              // r8
    sp[9]  = 0;              // r9
    sp[10] = 0;              // r10
    sp[11] = 0;              // r11
    sp[12] = 0;              // r12
    sp[13] = (uint32_t)entry; // lr: branch here when context is restored
}

/* Initialize the single process */
void scheduler_init(void) {
    init_process(&process_table[0], process1_entry, process1_stack);
    process_table[0].pid = 0;
    process_table[0].state = READY;
}

/* For one process scheduler_get_next always returns the same process */
PCB* scheduler_get_next(void) {
    return &process_table[0];
}

/* Yield the CPU:
   For one process this will simply reload the same process’s context.
 */
void yield(void) {
    next_pcb = scheduler_get_next();
    context_switch();
}

/* The kernel's main entry point */
void kernel_main(void) {
    uart_puts("Welcome to buddyOS\n");
    scheduler_init();
    if (init_alloc() >= 0) {
        uart_puts("MEMORY ALLOCATOR INIT\n");
    } else {
        uart_puts("MEMORY ALLOCATOR FAILED TO INIT\n");
    }

    /* For the initial context switch, set current_pcb to 0 so that the assembly code skips saving the kernel context */
    current_pcb = 0;
    next_pcb = &process_table[0];

    /* Load the process context.
       This pops the registers (including lr, which is set to process1_entry) and branches to process1_entry. */
    context_switch();

    /* Should never reach here; if we do, loop forever */
    while (1) { }
}

