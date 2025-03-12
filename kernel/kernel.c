#include "uart.h"
#include "proc.h"
#include "memory.h"        
#include <stdint.h>

/* Process function declarations */
extern void process1(void);
extern void process2(void);
extern void process3(void);

void kernel_main(void) {
    void *test;
    uart_puts("Welcome to BuddyOS\n");

    /* Initialize buddyOS memory allocator */
    if (init_alloc() >= 0) {
        uart_puts("MEMORY ALLOCATOR INIT\n");
    } else {
        uart_puts("MEMORY ALLOCATOR FAILED TO INIT\n");
    }
    if ((test = kmalloc(60)) == NULL) {
        uart_puts("fail\n");
    }
    if (kfree(test) == -1) {
        uart_puts("kfree fail\n");
    }
    
    /* Initialize three processes (using only the first three slots) */
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

