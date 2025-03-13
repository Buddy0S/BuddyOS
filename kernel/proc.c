#include "proc.h"
#include "uart.h"    
#include <stdint.h>
#include "memory.h"

/* Global arrays for PCBs and their stacks */
PCB PROC_TABLE[MAX_PROCS];
uint32_t PROC_STACKS[MAX_PROCS][STACK_SIZE];

/* Global variables for current process and the ready queue */
PCB *current_process;
struct KList ready_queue;

/* Initialize the ready queue */
void init_ready_queue(void) {
    list_init(&ready_queue);
}

/* Simple delay routine */
void delay(void) {
    for (volatile unsigned int i = 0; i < 1000000; i++);
}

/* Round-robin yield: switches context to the next process */
void yield(void) {
    int next_index = (current_index + 1) % MAX_PROCS;
    int old_index = current_index;
    current_index = next_index;  /* Update current process index before switching */
    switch_context((unsigned int **)&PROC_TABLE[old_index].stack_ptr,
                   (unsigned int **)&PROC_TABLE[next_index].stack_ptr);
}

/* Initialize a process's PCB so that when its context is restored, execution begins at func */
void init_process(PCB *p, void (*func)(void), uint32_t *stack_base, int pid, ProcessPriority prio) {
    /* Set basic PCB values */
    p->pid = pid;
    p->state = READY;
    p->prio = prio;
    p->exitStatus = 0;
    p->stack_base = stack_base;

    /* Initialize the list of this proc's children */
    list_init(&p->children);

    /* Set the stack pointer to the top of the process's stack */
    uint32_t *stack_top = stack_base + STACK_SIZE;

    /* Reserve space for registers r4–r11 and LR (9 words) */
    stack_top -= 9;

    /* Initialize registers r4–r11 with 0 */
    for (int i = 0; i < 8; i++) {
        stack_top[i] = 0;
    }

    /* Set the saved LR to the address of the process function;
       when the context is restored, execution will jump to func */
    stack_top[8] = (uint32_t) func;
    p->stack_ptr = stack_top;
}
