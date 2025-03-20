#include <stdint.h>
#include "uart.h"
#include "proc.h"

/* Round-robin yield: switches context to the next process */
void schedule(void) {
    PCB *current = current_process;
    
    /* Remove the head node and add it to the tail */
    struct KList *node = list_pop(&ready_queue);
    list_add_tail(&ready_queue, node);
    
    /* The new head of the ready queue is the next process */
    PCB *next = knode_data(list_first(&ready_queue), PCB, sched_node);
    current_process = next;
}

int32_t add_two_numbers(int a, int b) {
    return a + b;
}

void execute_syscall(uint32_t svc_num, uint32_t* args) {
    /* might refactor this */
    switch (svc_num) {
        case 0:
            uart0_printf("yield\n");
            break;
        case 1:
            uart0_printf("add two numbers together tye shi\n");
            args[0] = add_two_numbers(args[0], args[1]);
            break;
        default:
            uart0_printf("unknown\n");
            break;
    }

}


void dispatcher(void) {
    // run first process
    while (1) {
        uart0_printf("\nstart of dispatcher\n");
        uart0_printf("returned to dispatcher from process #%d\n", current_process->pid);


        uart0_printf("trap reason: ");
        switch (current_process->trap_reason) {
            case SYSCALL:
                uart0_printf("syscall #%d = ", current_process->syscall_num);
                /* call some sort of syscall handler here */
                execute_syscall(current_process->syscall_num, current_process->r_args);
                break;
            case INTERRUPT:
                uart0_printf("interrupt\n");
                break;
            case HANDLED:
                uart0_printf("time to die\n");
                break;
            default:
                uart0_printf("unknown\n");
                break;
        }
        /* if the process then returns to the dispatcher without 
         * going through a syscall or an interrupt then that means it just
         * returned from the function it started at which means it should 
         * probably just get killed */
        current_process->trap_reason = HANDLED;


        /* after dealing with last process, schedule next process to run */
        schedule();
        uart0_printf("jumping to %x for process #%d\n", current_process->context.lr, current_process->pid);
        if (current_process->started) {
            switch_to_svc(kernel_process, current_process);    
        } else {
            current_process->started = true;
            switch_to_start(kernel_process, current_process);    
        }

    }
}
