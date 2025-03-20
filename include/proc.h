#ifndef PROC_H
#define PROC_H

#include <stdint.h>
#include <stdbool.h>
#include <list.h>
#include <srr_ipc.h>

#define MAX_PROCS   3
#define STACK_SIZE  256
#define KERNEL_STACK_SIZE 256

/* Process states */
enum ProcessState {
    READY,
    RUNNING,
    BLOCKED,
    DEAD
};


/* Process priorities */
enum ProcessPriority {
  LOW,
  MEDIUM,
  HIGH
};


/* Process context */
typedef struct context {
    /* general purpose */
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
    int32_t pid;                    /* Process ID */
    int32_t ppid;                   /* Parent process ID */
    int32_t state;                  /* Process state */
    int32_t prio;                   /* Process priority */
    int32_t exitStatus;             /* Code/signal from when a process is interrupted */
    int32_t syscall_num;

    uint32_t *stack_base;           /* Base address of the allocated stack */
    uint32_t *stack_ptr;            /* Pointer to the saved context (stack pointer) */
    context context;                /* registers that need to be saved on context switch */
    uint32_t *saved_sp;            /* Pointer to the suspended process' sp (when in svc mode) */
    uint32_t saved_lr;            /* Pointer to the suspended process' lr (when in svc mode) */

    uint32_t *r_args;

    bool started;

    struct KList children;          /* A list of this proc's children */
    struct KList sched_node;        /* Node for the scheduler's ready queue */

    struct SRRMailbox mailbox;      /* Mailbox for IPC send recieve and reply */ 

} PCB;


/* Current running process pointer */
extern PCB *current_process;
extern PCB *kernel_process;

/* Ready queue for processes */
extern struct KList ready_queue;

/* Global process table and stacks */
extern PCB PROC_TABLE[MAX_PROCS];
extern uint32_t PROC_STACKS[MAX_PROCS][STACK_SIZE];
extern int current_index;

/* Function declarations */
void delay(void);
void yield(void);
void init_process(PCB *p, void (*func)(void), uint32_t *stack_base, int32_t prio);
void init_ready_queue(void);
extern void switch_to_svc(PCB *from, PCB *to);
extern void switch_to_start(PCB *from, PCB *to);
extern void switch_to_dispatch(PCB *from, PCB *to);

#endif /* PROC_H */
