#include <stdint.h>
#include "syscall.h"
#include "reg.h"
#include "uart.h"
#include "memory.h"
#include "list.h"
#include "proc.h"
#include "net.h"
#include "led.h"

/* Global arrays for PCBs and their stacks */
PCB PROC_TABLE[MAX_PROCS];
uint32_t PROC_STACKS[MAX_PROCS][STACK_SIZE];

/* Global variables for current process, kernel process, and the ready queue */
PCB *current_process;
PCB kernel_p;
PCB *kernel_process = &kernel_p;

struct KList ready_queue;

/* Initialize the ready queue */
void init_ready_queue(void) {
    list_init(&ready_queue);
}

/* Process function declarations */
extern void process1(void);
extern void process2(void);
extern void process3(void);

/* Function for delay */
void delay(void) {
    for (volatile unsigned int i = 0; i < 1000000; i++);
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

    /* Add this process to the ready queue */
    list_add_tail(&ready_queue, &p->sched_node);
}

/* test function that calls a syscall that takes 2 arguments */
int __syscalltest(int a, int b) {
    return SYSCALL(SYSCALL_TEST_2_ARGS_NR);
}

int __yield(void) {
    return SYSCALL(SYSCALL_YIELD_NR);
}

void process0(void) {
    uart0_printf("Process 0\n");
    while (1) {
        uart0_printf("\nProcess 0 received 5 + 10 = %d\n", __syscalltest(5, 10));
        delay();
    }
}

void process1(void) {
    uart0_printf("Process 1\n");
    while (1) {
        uart0_printf("\nProcess 1 is going to sleep\n");
        delay();
        WFI();
        uart0_printf("\nProcess 1 has been resurrected\n");
    }
}

void null_proc(void) {
    while (1) {
        uart0_printf("null proc going to sleep... zzzzzzz\n");
        WFI();
    }
}

extern void supervisor_call(void);
extern void dispatcher(void);

void buddy(void) {

    volatile int i;
    volatile int T = 500000;

    for (i = 0; i < T; i++);

    LEDon(LED1);

    for (i = 0; i < T; i++);

    LEDon(LED2);

    for (i = 0; i < T; i++);

    LEDon(LED3);

    for (i = 0; i < T; i++);

    LEDoff(LED0);

    for (i = 0; i < T; i++);

    LEDoff(LED1);

    for (i = 0; i < T; i++);

    LEDoff(LED2);

    for (i = 0; i < T; i++);

    LEDoff(LED3);

    for (i = 0; i < T; i++);

}


int main(){

    uart0_printf("Entering Kernel\n");
    register uint32_t sp asm("sp");
    uart0_printf("current sp: %x\n", sp);


    /* Initialize buddyOS memory allocator */
    if (init_alloc() >= 0) {
        uart0_printf("MEMORY ALLOCATOR INIT\n");
    } else {
        uart0_printf("MEMORY ALLOCATOR FAILED TO INIT\n");
    }

    
    /* Initialize the ready queue */
    init_ready_queue();

    /* Initialize three processes (using only the first three slots) with MEDIUM priority */
    init_process(&PROC_TABLE[0], process0, PROC_STACKS[0], MEDIUM);
    init_process(&PROC_TABLE[1], process1, PROC_STACKS[1], MEDIUM);

    uart0_printf("process gonan jump to %x\n", process1);

    /* Set the current process to the head of the ready queue */
    current_process = knode_data(list_first(&ready_queue), PCB, sched_node);

    /* Call dispatcher */
    dispatcher();

    while (1) {}	

    return 0;
}
