#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include <list.h>
#include <srr_ipc.h>

#define MAX_PROCS   3
#define STACK_SIZE  256
#define KERNEL_STACK_SIZE 256
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


/* Process context */
typedef struct context {
    /* general purpose */
    int32_t r0;
    int32_t r1;
    int32_t r2;
    int32_t r3;
    int32_t r4;
    int32_t r5;
    int32_t r6;
    int32_t r7;
    int32_t r8;
    int32_t r9;
    int32_t r10;
    int32_t r11;
    
    int32_t lr; /* link register */


} context;


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

    int exitStatus;             /* Code/signal from when a process is interrupted */

    struct SRRMailbox mailbox; /* Mailbox for IPC send recieve and reply */

    context context;

} PCB;

/* Current running proces pointer */
extern PCB *current_process;
extern PCB kernel_process;

/* Ready queue for processes */
extern struct KList ready_queue;

/* Global process table and stacks */
extern PCB PROC_TABLE[MAX_PROCS];
extern uint32_t PROC_STACKS[MAX_PROCS][STACK_SIZE];
extern int current_index;

/* Function declarations */
void delay(void);
void yield(void);
void init_process(PCB *p, void (*func)(void), uint32_t *stack_base, ProcessPriority prio);
void init_ready_queue(void);
extern void switch_context(unsigned int **old_sp, unsigned int **new_sp);

#endif /* PROC_H */


