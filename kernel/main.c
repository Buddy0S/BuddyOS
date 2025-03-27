#include <stdint.h>
#include "srr_ipc.h"
#include "syscall.h"
#include "reg.h"
#include "uart.h"
#include "memory.h"
#include "list.h"
#include "proc.h"
#include "fat12.h"
#include "vfs.h"
#include "net.h"
#include "led.h"
#include "vfs.h"

/* Process function declarations */
extern void process1(void);
extern void process2(void);
extern void process3(void);

/* Function for delay */
void delay(void) {
    for (volatile unsigned int i = 0; i < 1000000; i++);
}

/* test function that calls a syscall that takes 2 arguments */
int __syscalltest(int a, int b) {
    return SYSCALL(SYSCALL_TEST_2_ARGS_NR);
}

int __yield(void) {
    return SYSCALL(SYSCALL_YIELD_NR);
}

int __send_end(void* reply, uint32_t* rlen) {
    return SYSCALL(SYSCALL_SEND_END);
}

int __send_start(int pid, struct Mail* mail_in, void* reply, uint32_t* rlen) {
    return SYSCALL(SYSCALL_SEND_NR);
}

int __send(int pid, void *msg, uint32_t len, void* reply, uint32_t* rlen) {
    int result;
    struct Mail mail_in = {
        .msg = msg,
        .len = len
    };
    result = __send_start(pid, &mail_in, reply, rlen);
    if (result != 0) {
        uart0_printf("send failed: %d\n", result);
        return result;
    }

    return __send_end(reply, rlen);
}

int __receive_end(int* author, void* msg, uint32_t* len) {
    return SYSCALL(SYSCALL_RECEIVE_END);
}

int __receive_start(int* author, void* msg, uint32_t* len) {
    return SYSCALL(SYSCALL_RECEIVE_NR);
}

int __receive(int* author, void* msg, uint32_t* len) {
    int result;
    result = __receive_start(author, msg, len);
    if (result != 0) {
        return result;
    }

    return __receive_end(author, msg, len);
}

int __reply(int pid, void* msg, uint32_t len) {
    return SYSCALL(SYSCALL_REPLY_NR);
}

int __msg_waiting() {
    return SYSCALL(SYSCALL_MSG_WAITING_NR);
}

int __fork() {
	return SYSCALL(SYSCALL_FORK_NR);
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

    /* Initialize buddyOS memory allocator */
    if (init_alloc() >= 0) {
        uart0_printf("MEMORY ALLOCATOR INIT\n");
    } else {
        uart0_printf("MEMORY ALLOCATOR FAILED TO INIT\n");
    }
  
    /* Initialise all PCBs */
    for (int i = 0; i < MAX_PROCS; i++) {
    	PROC_TABLE[i].state = DEAD;
    }

  init_network_stack();

	/* ********* Test File system ********* */

	uint32_t* buffer = (uint32_t*)kmalloc(128 * sizeof(uint32_t));
	fat12_init(0, buffer);
	kfree(buffer);

	vfs_mount("/", FAT12);
	vfs_mount("/home", FAT12);

	char buf[64];

	int fd = vfs_open("/home/TEST.TXT", O_READ | O_WRITE);
	int bytes = vfs_read(fd, buf, 64);
	uart0_printf("%s (%d bytes)\n", buf, bytes);
	bytes = vfs_write(fd, "I ain't reading all that", sizeof("I ain't reading all that"));
	vfs_close(fd);

	fd = vfs_open("/home/HELLO.TXT", O_WRITE);
	bytes = vfs_write(fd, "Hello World!", sizeof("Hello World!"));
	uart0_printf("Wrote %d bytes\n", bytes);
	vfs_close(fd);

	fd = vfs_open("/home/NOT_EX.TXT", O_READ);
	bytes = vfs_write(fd, "Hello World!", sizeof("Hello World!"));
	uart0_printf("Wrote %d bytes\n", bytes);
	vfs_close(fd);

	fd = vfs_open("/home/DIS.TXT", O_WRITE);
	vfs_close(fd);
    
    
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
