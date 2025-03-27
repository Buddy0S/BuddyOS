#include <stdint.h>
#include <uart.h>
#include <proc.h>
#include <syscall.h>
#include <srr_ipc.h>
#include <memory.h>
#include <syserr.h>
#include "net.h"

/* Round-robin yield: switches context to the next process */
void schedule(void) {
    PCB *current = current_process;

    /* Remove the head node and add it to the tail */
    if (list_empty(&ready_queue)) {
        /* null proc */
        uart0_printf("going to null proc\n");
        current_process = &PROC_TABLE[0];
        return;
    } else if (current->prio != LOW) {
        if (current->state != BLOCKED) {
            struct KList *node = list_pop(&ready_queue);
            list_add_tail(&ready_queue, node);
        }
    }

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
        case SYSCALL_YIELD_NR:
#ifdef DEBUG
            uart0_printf("yield\n");
#endif
            schedule();
            break;
        case SYSCALL_TEST_2_ARGS_NR:
#ifdef DEBUG
            uart0_printf("add two numbers together tye shi\n");
#endif
            args[0] = add_two_numbers(args[0], args[1]);
            break;
        case SYSCALL_SEND_NR:
#ifdef DEBUG
            uart0_printf("IPC SEND\n");
#endif
            args[0] = send((int)args[0], (struct Mail*)args[1], (void*)args[2],
                    (uint32_t*) args[3]);
            break; 
        case SYSCALL_SEND_END:
#ifdef DEBUG
            uart0_printf("IPC SEND SECOND HALF\n");
#endif
            args[0] = send_end((void*)args[0], (uint32_t*)args[1]);
            break;
        case SYSCALL_RECEIVE_NR:
#ifdef DEBUG
            uart0_printf("IPC RECV\n");
#endif
            args[0] = receive((int*)args[0], (void*)args[1],
                    (uint32_t*)args[2]);
            break;
        case SYSCALL_RECEIVE_END:
#ifdef DEBUG
            uart0_printf("IPC RECV SECOND HALF\n");
#endif
            args[0] = receive_end((int*)args[0], (void*)args[1],
                    (uint32_t*)args[2]);
            break;
        case SYSCALL_REPLY_NR:
#ifdef DEBUG
            uart0_printf("IPC REPLY\n");
#endif
            args[0] = reply((int)args[0], (void*)args[1], (uint32_t)args[2]);
            break;
        case SYSCALL_MSG_WAITING_NR:
#ifdef DEBUG
            uart0_printf("IPC MSGWAITS\n");
#endif
            args[0] = msg_waiting();
            break;
        case SYSCALL_FORK_NR:
#ifdef DEBUG
            uart0_printf("fork syscall\n");
#endif
            args[0] = fork();
            break;
        case SYSCALL_SOCKET_NR:
        {
#ifdef DEBUG
            uart0_printf("SOCKET BRO\n");
#endif
            struct socket *soc = (struct socket*) args[0];

            args[0] = socket(soc->pid,soc->src_port,soc->dest_port,soc->dest_mac,soc->dest_ip,soc->protocol,soc->buddy_protocol);
        }
            break;
        case SYSCALL_SOCKET_BIND_NR:
        {
#ifdef DEBUG
            uart0_printf("SOCKET BIND BRO\n");
#endif
            int soc = (int) args[0];

            args[0] = socket_bind(soc);
        }
            break;
        case SYSCALL_SOCKET_UNBIND_NR:
        {
#ifdef DEBUG
            uart0_printf("SOCKET UNBIND BRO\n");
#endif
            int soc = (int) args[0];

            args[0] = socket_unbind(soc);
        }
            break;
        case SYSCALL_SOCKET_RECV_NR:
        {
#ifdef DEBUG
            uart0_printf("SOCKET RECV BRO\n");
#endif
            int soc = (int) args[0];

            args[0] = (uint32_t) socket_recv(soc);
        }
            break;
        case SYSCALL_SOCKET_REQUEST_NR:
        {
#ifdef DEBUG
            uart0_printf("SOCKET REQ BRO\n");
#endif
            int soc = (int) args[0];
            uint8_t* frame = (uint8_t*) args[1];
            int size = (int) args[2];

            args[0] = socket_transmit_request(soc, frame, size);
        }
            break;

        default:
#ifdef DEBUG
            uart0_printf("unknown/unimplemented\n");
#endif
            args[0] = -ENOSYS;
            break;
    }

}


void dispatcher(void) {
    // run first process
#ifdef DEBUG
    uart0_printf("\nstart of dispatcher\n");
#endif
    while (1) {
        if (current_process->started) {
            if (current_process->trap_reason == SYSCALL) {
                /* if the process then returns to the dispatcher without 
                 * going through a syscall or an interrupt then that means it just
                 * returned from the function it started at which means it should 
                 * probably just get killed */
#ifdef DEBUG
                uart0_printf("svc\n");
#endif
                current_process->trap_reason = HANDLED;
                switch_to_svc(kernel_process, current_process);    
            } else if (current_process->trap_reason == INTERRUPT) {
#ifdef DEBUG
                uart0_printf("interrupt\n");
#endif
                current_process->trap_reason = HANDLED;
                switch_to_irq(kernel_process, current_process);
            }
            /* check for handled and kill process */
        } else {
            current_process->started = true;
            switch_to_start(kernel_process, current_process);    
        }
#ifdef DEBUG
        uart0_printf("\nreturned to dispatcher from process #%d\n", current_process->pid);
        uart0_printf("trap reason: ");
#endif

        switch (current_process->trap_reason) {
            case SYSCALL:
#ifdef DEBUG
                uart0_printf("syscall #%d = ", current_process->status);
#endif
                /* call some sort of syscall handler here */
                execute_syscall(current_process->status, current_process->r_args);
                break;
            case INTERRUPT:
#ifdef DEBUG
                uart0_printf("interrupt #%d\n", current_process->status);
#endif
                break;
            case HANDLED:
#ifdef DEBUG
                uart0_printf("time to die\n");
#endif
                break;
            default:
#ifdef DEBUG
                uart0_printf("unknown\n");
#endif
                break;
        }

        /* after dealing with last process, schedule next process to run */
        if (current_process->state == BLOCKED) {
            current_process->cpu_time = PROC_QUANTUM;
            current_process->quantum_elapsed = false;
            schedule();
        }
        else if (current_process->quantum_elapsed) {
            current_process->cpu_time = PROC_QUANTUM;
            current_process->quantum_elapsed = false;
            schedule();
        }
#ifdef DEBUG
        uart0_printf("jumping to %x for process #%d through ", current_process->context.lr, current_process->pid);
#endif

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

int32_t fork(void) {
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
    kmemcpy(parent->stack_base, child_stack, STACK_SIZE * sizeof(uint32_t));


    // use the same offset into the child's stack as the parent's saved_sp 
    // had into the parent's stack.
    // Set child's stack pointer (same offset as parent)
    int32_t parent_stack_offset = parent->saved_sp - parent->stack_base;
    child->saved_sp = child_stack + parent_stack_offset;

    // Keep the saved LR the same as parent's
    child->saved_lr = parent->saved_lr;

    // Copy parent's saved context (registers r4-r11, lr)
    child->context = parent->context;

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
    child->started = true; 
    child->trap_reason = SYSCALL;
    child->cpu_time = PROC_QUANTUM;
    child->quantum_elapsed = false;
    child->r_args = (uint32_t*)child_args;
    // Adjust child's saved r0 (syscall return value) to 0
    child->r_args[0] = 0;

    // Initialize child's mail
    srr_init_mailbox(&child->mailbox);

    // Add child to ready queue hopefully works lol
    list_add_tail(&ready_queue, &child->sched_node);

#ifdef DEBUG
    uart0_printf("fork: child PID %d\n", child_pid);
#endif
    return child_pid;
}
