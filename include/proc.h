#ifndef PROC_H
#define PROC_H

#include <stdint.h>

/* Process states */
typedef enum {
    READY,
    RUNNING,
    /* We can add more states like BLOCKED, etc, later */
} ProcessState;

/* Process Control Block (PCB) definition */
typedef struct PCB {
    int pid;              /* Process ID */
    ProcessState state;   /* Process state */
    uint32_t *stack_ptr;  /* Pointer to the saved context (stack pointer) */
    uint32_t *stack_base; /* Base address of the allocated stack */
} PCB;

#endif



