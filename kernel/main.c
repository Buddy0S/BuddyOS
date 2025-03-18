#include <stdint.h>
#include "syscall.h"
#include "reg.h"
#include "uart.h"
#include "memory.h"
#include "list.h"
#include "proc.h"
#include "vfs.h"
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

    /* Initialize registers r4–r11 with 0 */
    for (int i = 0; i < 8; i++) {
        ((uint32_t*)(&p->context))[i] = 0;

    }

    /* Initialize saved SPSR (with interrupts enabled) into stored SPSR.
       This ensures that when the process context is restored, it resumes with the proper CPSR */
    asm volatile("  \n\t    \
       mrs r0, spsr     \n\t    \
       bic r0, r0, #0x1F\n\t    \
       orr r0, r0, #0x10\n\t    \
            ");
    register uint32_t r0 asm("r0");
    p->cpsr = r0;

    /* Set the saved LR to the address of the process function;
       when the context is restored, execution will jump to func */
    p->context.lr = (int32_t)func;

    /* Set the stack pointer to the top of the process's stack */
    p->stack_ptr = stack_base + STACK_SIZE;

    p->started = false;

    /* Add this process to the ready queue */
    list_add_tail(&ready_queue, &p->sched_node);
}

// these syscalls 1-3 do not have return values, so they will print the
// value that was in r0 when they were called

void process1(void) {
    while (1) {
        uart0_printf("Process 1\n");
        register uint32_t sp asm("sp");
        uart0_printf("current sp: %x\n", sp);
        register uint32_t r0 asm("r0");
        asm volatile("  \n\t    \
                mrs r0, cpsr     \n\t    \
                "::: "r0");
        uart0_printf("current cpsr: %x\n", r0);
        delay();
        SYSCALL(1);
    }
}

void process2(void) {
    while (1) {
        uart0_printf("Process 2\n");
        register uint32_t sp asm("sp");
        uart0_printf("current sp: %x\n", sp);
        register uint32_t r0 asm("r0");
        asm volatile("  \n\t    \
                mrs r0, cpsr     \n\t    \
                "::: "r0");
        uart0_printf("current cpsr: %x\n", r0);

        delay();
        SYSCALL(1);
    }
}

void process3(void) {
    while (1) {
        uart0_printf("Process 3\n");
        register uint32_t sp asm("sp");
        uart0_printf("current sp: %x\n", sp);
        delay();
        SYSCALL(1);
    }
}

/* test function that calls a syscall that takes 2 arguments */
int test_syscall_sum(int a, int b) {
    return SYSCALL(0);
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

	/* ********* Test File system ********* */
	
	vfs_mount("/", FAT12);
	vfs_mount("/home", FAT12);

	vfs_open("/home/user/folder/file", 2);

	//uart0_printf("FS TEST = %s(%d)\n", mnt->fs_mountpoint, mnt->type);

	/*****************************************/
    
    cpsw_init();

    phy_init(); 
    
	init_network_stack(); 
    
    /* Initialize the ready queue */
    init_ready_queue();

    /* Initialize three processes (using only the first three slots) with MEDIUM priority */
    init_process(&PROC_TABLE[0], process1, PROC_STACKS[0], MEDIUM);

    uart0_printf("process gonan jump to %x\n", process2);

    /* Set the current process to the head of the ready queue */
    current_process = knode_data(list_first(&ready_queue), PCB, sched_node);

    /* Call dispatcher */
    dispatcher();

    while (1) {}	

    return 0;
}
