#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

/* GPIO */
#define UART0_BASE 0x44E09000

/*MEMORY*/
#define KERNEL_START        0x80000000
#define KERNEL_SIZE         0x00400000  /* 4MB */
#define KERNEL_RESERVED     0x00400000  /* 4MB */
#define KERNEL_DYNAMIC      0x00400000  /* 4MB */

#define KERNEL_TOTAL        (KERNEL_START + KERNEL_SIZE + KERNEL_RESERVED + KERNEL_DYNAMIC)

#define KERNEL_RESERVED_START (KERNEL_START + KERNEL_SIZE)
#define KERNEL_DYNAMIC_START (KERNEL_RESERVED_START + KERNEL_RESERVED)

#define ALLOC_START         (KERNEL_START + KERNEL_TOTAL)
#define ALLOC_END           0x8FFFFFFF

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


void kputc(char c);
void kputs(const char *s);

int init_alloc(void);
void *kmalloc(uint32_t size);
int kfree(void *ptr);


#endif
