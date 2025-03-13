#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include <list.h>

#define MAX_PROCS   3
#define STACK_SIZE  1024
#define NULL ((void *)0)

/* Process states */
typedef enum {
    READY,
    RUNNING,
    BLOCKED,
    DEAD
} ProcessState;

/* Process priorities */
typedef enum {
  LOW,
  MEDIUM,
  HIGH
} ProcessPriority;

/* Process Control Block (PCB) definition */
typedef struct PCB {
    int pid;                  /* Process ID */
    int ppid;                 /* Parent process ID */
    ProcessState state;       /* Process state */
    ProcessPriority prio;      /* Process priority */

    struct KList children;     /* A list of this proc's children */
    struct KList sched_node;   /* Node for the scheduler's ready queue */

    uint32_t *stack_ptr;      /* Pointer to the saved context (stack pointer) */
    uint32_t *stack_base;     /* Base address of the allocated stack */

    uint32_t *kernel_sp;      /* Kernel stack pointer for saving kernel context */

    int exitStatus;             /* Code/signal from when a process is interrupted */

} PCB;

/* Current running proces pointer */
extern PCB *current_process;

/* Ready queue for processes */
extern struct KList ready_queue;

/* Global process table and stacks */
extern PCB PROC_TABLE[MAX_PROCS];
extern uint32_t PROC_STACKS[MAX_PROCS][STACK_SIZE];
extern int current_indexi;

/* Function declarations */
void delay(void);
void yield(void);
void init_process(PCB *p, void (*func)(void), uint32_t *stack_base, int pid, ProcessPriority prio);
void init_ready_queue(void);
extern void switch_context(unsigned int **old_sp, unsigned int **new_sp);

#endif /* PROC_H */

