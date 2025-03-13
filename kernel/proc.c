#include "proc.h"
#include "uart.h"    
#include <stdint.h>
#include "memory.h"

/* Global arrays for PCBs and their stacks */
PCB PROC_TABLE[MAX_PROCS];
uint32_t PROC_STACKS[MAX_PROCS][STACK_SIZE];
uint32_t KERNEL_STACKS[MAX_PROCS][KERNEL_STACK_SIZE];

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
    
    delay();

    PCB *current = current_process;
    
    /* Remove the head node and add it to the tail */
    struct KList *node = list_pop(&ready_queue);
    list_add_tail(&ready_queue, node);
    
    /* The new head of the ready queue is the next process */
    PCB *next = knode_data(list_first(&ready_queue), PCB, sched_node);
    current_process = next;
    
    /* Switch context from current process to the next process */
    switch_context((unsigned int **)&current->stack_ptr,
                   (unsigned int **)&next->stack_ptr);
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

    /* Add this process to the ready queue */
    list_add_tail(&ready_queue, &p->sched_node);
}
