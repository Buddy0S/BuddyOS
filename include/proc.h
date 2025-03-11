#ifndef PROC_H
#define PROC_H

#include <stdint.h>

#define MAX_PROCS   3
#define STACK_SIZE  1024
#define NULL ((void *)0)

/* Process states */
typedef enum {
    READY,
    RUNNING,
    /* Additional states (e.g., BLOCKED) can be added later */
} ProcessState;

/* Process Control Block (PCB) definition */
typedef struct PCB {
    int pid;              /* Process ID */
    ProcessState state;   /* Process state */
    uint32_t *stack_ptr;  /* Pointer to the saved context (stack pointer) */
    uint32_t *stack_base; /* Base address of the allocated stack */
} PCB;

/* Global process table and stacks */
extern PCB pcb[MAX_PROCS];
extern uint32_t proc_stacks[MAX_PROCS][STACK_SIZE];
extern int current_index;

/* Function declarations */
void delay(void);
void yield(void);
void init_process(PCB *p, void (*func)(void), uint32_t *stack_base, int pid);
extern void switch_context(unsigned int **old_sp, unsigned int **new_sp);

#endif /* PROC_H */

