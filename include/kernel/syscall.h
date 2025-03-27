#ifndef SYSCALL_H
#define SYSCALL_H

#include "proc.h"

/* lets us pass a macro to SYSCALL(x) */
#define STR(x) #x

#define SYSCALL(x)                          \
({                                          \
    register int32_t return_val asm("r0");  \
    asm volatile ("svc #" STR(x) "  \n\t"); \
    return_val;                             \
})                                          \

#define WFI()                               \
({                                          \
    asm volatile ("wfi  \n\t");             \
})  

#define SYSCALL_TEST_2_ARGS_NR  0
#define SYSCALL_YIELD_NR        1
#define SYSCALL_MALLOC_NR       2
#define SYSCALL_FORK_NR         3
#define SYSCALL_KILL_NR         4
#define SYSCALL_GETPID_NR       5
#define SYSCALL_SEND_NR         6
#define SYSCALL_RECEIVE_NR      7
#define SYSCALL_REPLY_NR        8
#define SYSCALL_MSG_WAITING_NR  9
#define SYSCALL_SEND_END        10
#define SYSCALL_RECEIVE_END     11
#define SYSCALL_EXIT_NR         12

/* test function that calls a syscall that takes 2 arguments */
int __syscalltest(int a, int b);

int __yield(void);

int __send_end(void* reply, uint32_t* rlen);

int __send_start(int pid, struct Mail* mail_in, void* reply, uint32_t* rlen);

int __send(int pid, void *msg, uint32_t len, void* reply, uint32_t* rlen);

int __receive_end(int* author, void* msg, uint32_t* len);

int __receive_start(int* author, void* msg, uint32_t* len);

int __receive(int* author, void* msg, uint32_t* len);

int __reply(int pid, void* msg, uint32_t len);

int __msg_waiting();

int __fork();

void __exit();

#endif
