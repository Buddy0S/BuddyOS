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

    /* Initialize the ready queue */
    init_ready_queue();
    
    /* Initialize three processes (using only the first three slots) */
    init_process(&PROC_TABLE[0], process1, PROC_STACKS[0], 0, MEDIUM);
    init_process(&PROC_TABLE[1], process2, PROC_STACKS[1], 1, MEDIUM);
    init_process(&PROC_TABLE[2], process3, PROC_STACKS[2], 2, MEDIUM);

    /* Set the current process to the head of the ready queue */
    current_process = knode_data(list_first(&ready_queue), PCB, sched_node);

    /* Save the kernel stack inside the PCB instead of a dummy variable */
    current_process->kernel_sp = KERNEL_STACKS[current_process->pid] + KERNEL_STACK_SIZE;

    /* Switch to the first process */
    switch_context((unsigned int **)&current_process->kernel_sp, 
		    (unsigned int **)&current_process->stack_ptr);    

    /* Should never reach here */
    while (1);
}

