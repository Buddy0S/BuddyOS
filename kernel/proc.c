#include <stdint.h>
#include "proc.h"

/* Global arrays for PCBs and their stacks */
PCB PROC_TABLE[MAX_PROCS];
uint32_t PROC_STACKS[MAX_PROCS][STACK_SIZE];
uint8_t *KERNEL_STACK_TOP = (uint8_t*)0x40300000; 

/* Global variables for current process, kernel process, and the ready queue */
PCB *current_process;
PCB **curr_ptr = &current_process;
PCB kernel_p;
PCB *kernel_process = &kernel_p;

struct KList ready_queue;

/* Initialize the ready queue */
void init_ready_queue(void) {
    list_init(&ready_queue);
}

/* Initialize a process's PCB so that when its context is restored, execution begins at func */
void init_process(PCB *p, void (*func)(void), uint32_t *stack_base, int32_t prio) {
    /* Set basic PCB values */
    p->pid = p - PROC_TABLE;
    p->state = READY;
    p->prio = prio;
    p->exitStatus = 0;
    p->stack_base = stack_base;

    /* Initialize the list of this proc's children */
    list_init(&p->children);

    /* Set the saved LR to the address of the process function;
       when the context is restored, execution will jump to func */
    p->context.lr = (int32_t)func;

    /* Set the stack pointer to the top of the process's stack */
    p->stack_ptr = stack_base + STACK_SIZE;

    /* Make sure started is false, so the dispatch switches into proc properly */
    p->started = false;

    /* This way if it returns to the dispatcher on its own without syscalling
     * or being interrupted then we just know that the process is done */
    p->trap_reason = HANDLED;

    /* start all processes with a 100 ms quantum for now */
    p->cpu_time = PROC_QUANTUM;

    p->exception_stack_top = (uint32_t*)KERNEL_STACK_TOP - (p->pid * KERNEL_STACK_SIZE);

    srr_init_mailbox(&p->mailbox);

    /* Add this process to the ready queue */
    list_add_tail(&ready_queue, &p->sched_node);
}
