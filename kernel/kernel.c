#include "uart.h"
#include "proc.h"
#include "mm.h"        
#include <stdint.h>

#define STACK_SIZE 1024
#define NULL ((void *)0)

/* Arrays for PCBs and their stacks */
PCB pcb[3];
uint32_t proc_stacks[3][STACK_SIZE];

int current_index = 0;

/* Externally defined context switch routine (in context_switch.S) */
extern void switch_context(unsigned int **old_sp, unsigned int **new_sp);

/* declarations for helper routines */
void delay(void);
void yield(void);

/* Process function declarations */
extern void process1(void);
extern void process2(void);
extern void process3(void);

/* Function for delay */
void delay(void) {
    for (volatile unsigned int i = 0; i < 1000000; i++);
}

/* The function switches context to the next process in an RR style, later
 * we'll use a list for this */
void yield(void) {
    int next_index = (current_index + 1) % 3;
    int old_index = current_index;
    current_index = next_index;  /* Update current process index before switching */
    switch_context((unsigned int **)&pcb[old_index].stack_ptr,
                   (unsigned int **)&pcb[next_index].stack_ptr);
}

/* Initialize a process’s PCB so that when restored it begins execution at func */
void init_process(PCB *p, void (*func)(void), uint32_t *stack_base, int pid) {
    p->pid = pid;
    p->state = READY;
    p->stack_base = stack_base;

    /* Set the stack pointer to the top of the process's stack */
    uint32_t *stack_top = stack_base + STACK_SIZE;
    /* Reserve space for registers r4–r11 and LR */
    stack_top -= 9;
    /* Initialize registers r4–r11 with 0 as the processes are nothing right now*/
    for (int i = 0; i < 8; i++) {
        stack_top[i] = 0;
    }
    /* Set the saved LR to the address of the process function;
       when context is restored, execution will jump to func */
    stack_top[8] = (uint32_t)func;
    p->stack_ptr = stack_top;
}

/* Kernel entry point */
void kernel_main(void) {
    void *test;
    uart_puts("Welcome to BuddyOS\n");

    /* Initialize buddyOS memory allocator */
    if (init_alloc() >= 0) {
        uart_puts("MEMORY ALLOCATOR INIT\n");
    } else {
        uart_puts("MEMORY ALLOCATOR FAILED TO INIT\n");
    }
    if ((test = kmalloc(4)) == NULL) {
        uart_puts("fail");
    }
    if (kfree(test) == -1) {
        uart_puts("kfree fail");
    }
    /* Initialize the three processes */
    init_process(&pcb[0], process1, proc_stacks[0], 0);
    init_process(&pcb[1], process2, proc_stacks[1], 1);
    init_process(&pcb[2], process3, proc_stacks[2], 2);

    /* Save the kernel context in a dummy variable and switch to process 1.
       Execution will jump to process1 via its saved LR. */
    unsigned int *kernel_sp;
    switch_context(&kernel_sp, (unsigned int **)&pcb[0].stack_ptr);

    /* Should never reach here */
    while (1);
}

