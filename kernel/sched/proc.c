#include <stddef.h>
#include <stdint.h>
#include "memory.h"
#include "proc.h"
#include "syscall.h"
#include "uart.h"
#include "net.h"
#include "memory.h"
#include "vfs.h"

uint32_t gateway_ip = 0xC0A8010A;
uint8_t gateway_mac[MAC_ADDR_LEN] = {0xD8,0xBB,0xC1,0xF7,0xD0,0xD3};

/* Global arrays for PCBs and their stacks */
PCB PROC_TABLE[MAX_PROCS];
uint32_t PROC_STACKS[MAX_PROCS][STACK_SIZE];

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
    p->context.r4 = (int32_t)func;

    /* Set the stack pointer to the top of the process's stack */
    p->stack_ptr = stack_base + STACK_SIZE;

    /* Make sure started is false, so the dispatch switches into proc properly */
    p->started = false;

    /* This way if it returns to the dispatcher on its own without syscalling
     * or being interrupted then we just know that the process is done */
    p->trap_reason = HANDLED;

    /* start all processes with a 100 ms quantum for now */
    p->cpu_time = PROC_QUANTUM;

    p->exception_stack_top = (uint32_t*)EXCEPTION_STACK_TOP - (p->pid * EXCEPTION_STACK_SIZE);

    srr_init_mailbox(&p->mailbox);

    /* Add this process to the ready queue */
    if (prio != LOW) {
        list_add_tail(&ready_queue, &p->sched_node);
    }
}

PCB* get_PCB(int pid) {
    if (pid >= 0 && pid < MAX_PROCS) {
        return &PROC_TABLE[pid];
    }

    return NULL;
}

void wake_proc(int pid) {
    PCB* pcb;
    pcb = get_PCB(pid);
    if (pcb == NULL) {
        return;
    }

    if (pcb->state != READY) {
        pcb->state = READY;
        list_add_tail(&ready_queue, &pcb->sched_node);
    }
}

void block() {
    current_process->state = BLOCKED;
    list_pop(&ready_queue); /* clears the current process out of the queue */
}

int32_t create_process(void) {
    // Find a free PCB (state == DEAD)
    int child_pid = -1;
    for (int i = 0; i < MAX_PROCS; i++) {
        if (PROC_TABLE[i].state == DEAD) {
            child_pid = i;
            break;
        }
    }

    PCB *parent = current_process;
    if (child_pid == -1) return -1; // No free process slot

    PCB *child = &PROC_TABLE[child_pid];

    // Copy parent's stack to child's stack
    uint32_t *child_stack = PROC_STACKS[child_pid];

    /* get svc stack for child */
    child->exception_stack_top = (uint32_t*)EXCEPTION_STACK_TOP - (child_pid * EXCEPTION_STACK_SIZE);
    child->stack_ptr = child->exception_stack_top - (parent->exception_stack_top - parent->stack_ptr);
    uint32_t *child_args = (child->stack_ptr + (parent->r_args - parent->stack_ptr));
    kmemcpy(parent->exception_stack_top - EXCEPTION_STACK_SIZE, 
            child->exception_stack_top - EXCEPTION_STACK_SIZE, sizeof(uint32_t) * EXCEPTION_STACK_SIZE);

    // Initialize child's PCB
    child->pid = child_pid;
    child->ppid = parent->pid;
    child->state = READY;
    child->prio = parent->prio;
    child->stack_base = child_stack;
    child->cpu_time = PROC_QUANTUM;
    child->quantum_elapsed = false;
    child->r_args = (uint32_t*)child_args;

    // Initialize child's mail
    srr_init_mailbox(&child->mailbox);

    // Add child to ready queue hopefully works lol
    list_add_tail(&ready_queue, &child->sched_node);

#ifdef DEBUG
    uart0_printf("fork: child PID %d\n", child_pid);
#endif
    return child_pid;

}

int32_t fork(void) {

    PCB* child;
    PCB *parent = current_process;
    int child_pid = create_process();

    if (child_pid < 0) {
        return -1;
    }


    child = &PROC_TABLE[child_pid];

    // Copy parent's stack to child's stack
    kmemcpy(parent->stack_base, child->stack_base, STACK_SIZE * sizeof(uint32_t));

    // use the same offset into the child's stack as the parent's saved_sp 
    // had into the parent's stack.
    // Set child's stack pointer (same offset as parent)
    int32_t parent_stack_offset = parent->saved_sp - parent->stack_base;
    child->saved_sp = child->stack_base + parent_stack_offset;

    // Keep the saved LR the same as parent's
    child->saved_lr = parent->saved_lr;

    // Copy parent's saved context (registers r4-r11, lr)
    child->context = parent->context;

    // Initialize child's PCB
    child->started = true; 
    child->text_owner = false;
    child->trap_reason = SYSCALL;
    child->r_args[0] = 0;

#ifdef DEBUG
    uart0_printf("fork: child PID %d\n", child_pid);
#endif
    return child_pid;
}

int32_t f_exec(char * const path) {

    PCB* child;
    int fd;
    char* program;
    int child_pid;
    uint32_t filesize;

    fd = vfs_open(path, O_READ);
    if (fd < 0) {
        return -1;
    }

    filesize = vfs_getFileSize(fd);

    program = kmalloc(filesize);
    if (program == NULL) {
        vfs_close(fd);
        return -1;
    }

    child_pid = create_process();
    if (child_pid < 0) {
        kfree(program);
        vfs_close(fd);
        return -1;
    }

    vfs_read(fd, program, filesize);

    child = &PROC_TABLE[child_pid];
    child->started = false;
    child->trap_reason = HANDLED;
    child->saved_sp = child->stack_base + STACK_SIZE * sizeof(uint32_t);
    child->text_owner = true;
    child->text_ptr = (uint32_t)program;
    child->context.r4 = (int32_t)program;
    vfs_close(fd);

    return 0;
}

int32_t kexit(void) {
    current_process->state = DEAD;
    if(current_process->text_owner) {
        kfree((void*)current_process->text_ptr);
    }
    list_pop(&ready_queue); /* clears the current process out of the queue */
    return 0;
}

void proc_wrapper(void (*func)(void)) {
    func();
    __exit();
}

/*-----------------------TESTPROCS-----------------------*/

void null_proc(void) {
    while (1) {
        uart0_printf("null proc going to sleep... zzzzzzz\n");
        WFI();
        delay();
    }
}

void process1(void) {
    int sender_pid;
    char msg[20];
    uint32_t len;
    int pid = current_process->pid; 

    uart0_printf("Process 1 (Consumer) started. PID: %d\n", pid);

    while (1) {
        uart0_printf("\nConsumer PID %d: Waiting for a message...\n", pid);
        len = 20;  // reset length before each receive

        if (__receive(&sender_pid, msg, &len) == 0) {
            uart0_printf("Consumer PID %d: Received '%s' from sender PID %d\n", 
                         pid, msg, sender_pid);
            if (__reply(sender_pid, "ACK", 4) == 0) {
                uart0_printf("Consumer PID %d: Replied 'ACK' to sender PID %d\n", 
                             pid, sender_pid);
            } else {
                uart0_printf("Consumer PID %d: Failed to reply to sender PID %d\n", 
                             pid, sender_pid);
            }
        } else {
            uart0_printf("Consumer PID %d: Error receiving message\n", pid);
        }
        /* Im use a longer delay to simulate a slow consumer */
        delay();
        delay();
    }
}


void process2(void) {
    char message1[20] = "Hello buddy #1";
    char message2[20] = "Hello buddy #2";
    char response[20];
    uint32_t rsp_len;
    int consumer_pid = 1;  // process 1 pid better be 1 man
    int pid = current_process->pid;

    uart0_printf("Process 2 (Producer) started. PID: %d\n", pid);

    while (1) {
        uart0_printf("\nProducer PID %d: Preparing to send messages to Consumer PID %d\n",
                     pid, consumer_pid);

        /* Send first message */
        rsp_len = 20;
        uart0_printf("Producer PID %d: Sending: '%s'\n", pid, message1);
        if (__send(consumer_pid, message1, 20, response, &rsp_len) == 0) {
            uart0_printf("Producer PID %d: Received reply: '%s'\n", pid, response);
        } else {
            uart0_printf("Producer PID %d: Failed to send message 1\n", pid);
        }

        /* Send second message immediately after the first */
        rsp_len = 20;
        uart0_printf("Producer PID %d: Sending: '%s'\n", pid, message2);
        if (__send(consumer_pid, message2, 20, response, &rsp_len) == 0) {
            uart0_printf("Producer PID %d: Received reply: '%s'\n", pid, response);
        } else {
            uart0_printf("Producer PID %d: Failed to send message 2\n", pid);
        }

        /* Short delay between the bursts of two messages */
        delay();
    }
}

