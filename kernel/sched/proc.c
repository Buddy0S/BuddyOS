#include <stdint.h>
#include "proc.h"
#include "syscall.h"
#include "uart.h"

/* Global arrays for PCBs and their stacks */
PCB PROC_TABLE[MAX_PROCS];
uint32_t PROC_STACKS[MAX_PROCS][STACK_SIZE];
uint8_t *KERNEL_STACK_TOP = (uint8_t*)0x40300000; 

/* Global variables for current process, kernel process, and the ready queue */
PCB *current_process;
PCB **curr_ptr = &current_process;
PCB kernel_p;
PCB *kernel_process = &kernel_p;

struct KList ready_queue;

/* Initialize the ready queue */
void init_ready_queue(void) {
    list_init(&ready_queue);
}

/* Initialize a process's PCB so that when its context is restored, execution begins at func */
void init_process(PCB *p, void (*func)(void), uint32_t *stack_base, int32_t prio) {
    /* Set basic PCB values */
    p->pid = p - PROC_TABLE;
    p->state = READY;
    p->prio = prio;
    p->exitStatus = 0;
    p->stack_base = stack_base;

    /* Initialize the list of this proc's children */
    list_init(&p->children);

    /* Set the saved LR to the address of the process function;
       when the context is restored, execution will jump to func */
    p->context.lr = (int32_t)func;

    /* Set the stack pointer to the top of the process's stack */
    p->stack_ptr = stack_base + STACK_SIZE;

    /* Make sure started is false, so the dispatch switches into proc properly */
    p->started = false;

    /* This way if it returns to the dispatcher on its own without syscalling
     * or being interrupted then we just know that the process is done */
    p->trap_reason = HANDLED;

    /* start all processes with a 100 ms quantum for now */
    p->cpu_time = PROC_QUANTUM;

    p->exception_stack_top = (uint32_t*)KERNEL_STACK_TOP - (p->pid * KERNEL_STACK_SIZE);

    srr_init_mailbox(&p->mailbox);

    /* Add this process to the ready queue */
    list_add_tail(&ready_queue, &p->sched_node);
}


/*-----------------------TESTPROCS-----------------------*/

void null_proc(void) {
    while (1) {
        uart0_printf("null proc going to sleep... zzzzzzz\n");
        WFI();
        delay();
    }
}

void process0(void) {
    
    // Call fork and capture its return value.
    uart0_printf("Process 0\n");
    int fork_result = __fork();

    // Check the result
    if (fork_result == -1) {
        uart0_printf("Process 0: fork failed!\n");
    } else if (fork_result == 0) {
        // This branch is executed in the child
        uart0_printf("Process 0 (child): My PID is %d\n", current_process->pid);
        delay();
    } else {
        // This branch is executed in the paren
        uart0_printf("Process 0 (parent): fork returned child PID %x\n", fork_result);
    }
	
    int author;
    char msg[20];
    uint32_t len;
    int pid = current_process->pid;

    len = 20;

    while (1) {
        uart0_printf("\nProcess %d received 5 + 10 = %d\n", pid, __syscalltest(5, 10));
        delay();
        if (__msg_waiting()) {
            uart0_printf("Proc %d: Theres a message from my buddy!\n", pid);
        } else {
            uart0_printf("Proc %d: Theres no message from my buddy yet but thats fine ill wait\n", pid);
        }

        __receive(&author, msg, &len);
        uart0_printf("Proc %d: I got my buddy's message!\n", pid);
        uart0_printf("Proc %d: msg received from %d:\n\t%s\n", pid, author, msg);
        __reply(author, msg, 19);

        uart0_printf("Proc %d: sent my buddy a reply\n", pid);
    }
}

void process1(void) {
    char message[20] = "Hey buddy";
    char response[20];
    uint32_t rsp_len;
    int dest = 0;
    uart0_printf("Process 1\n");
    int fork_result = __fork();

    // Check the result
    if (fork_result == -1) {
        uart0_printf("Process 1: fork failed!\n");
    } else if (fork_result == 0) {
        // This branch is executed in the child
        uart0_printf("Process 1 (child): My PID is %d\n", current_process->pid);
        dest = 2;
    } else {
        // This branch is executed in the paren
        uart0_printf("Process 1 (parent): fork returned child PID %x\n", fork_result);
    }

    int pid = current_process->pid;

    while (1) {
        uart0_printf("\nProcess %d is going to sleep\n", pid);
        delay();
        WFI();
        uart0_printf("\nProcess %d has been resurrected\n", pid);
        uart0_printf("Proc %d: Sending a message to my buddy :)\n", pid);
        rsp_len = 20;
        __send(dest, message, 20, response, &rsp_len);
        uart0_printf("PROC%d: My buddy received my message!!\n", pid);
        uart0_printf("Proc %d: receiving %s\n", pid, response);
    }
}
