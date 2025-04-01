#ifndef SYSCALL_H
#define SYSCALL_H

#include "proc.h"
#include "net.h"

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

#define SYSCALL_TEST_2_ARGS_NR      0
#define SYSCALL_YIELD_NR            1
#define SYSCALL_MALLOC_NR           2
#define SYSCALL_FORK_NR             3
#define SYSCALL_KILL_NR             4
#define SYSCALL_GETPID_NR           5
#define SYSCALL_SEND_NR             6
#define SYSCALL_RECEIVE_NR          7
#define SYSCALL_REPLY_NR            8
#define SYSCALL_MSG_WAITING_NR      9
#define SYSCALL_SEND_END            10
#define SYSCALL_RECEIVE_END         11
#define SYSCALL_SOCKET_NR           12
#define SYSCALL_SOCKET_BIND_NR      13
#define SYSCALL_SOCKET_CLOSE_NR    14
#define SYSCALL_SOCKET_RECV_NR      15
#define SYSCALL_SOCKET_REQUEST_NR   16
#define SYSCALL_EXIT_NR             17
#define SYSCALL_F_EXEC_NR           18

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

int __socket(int pid, uint8_t* gateway, uint8_t protocol);

int __bind(int soc, socket_info *soc_info);

int __closesocket(int soc);

int __recvfrom(int soc, uint8_t* buff);

int __sendto(int soc, uint8_t* frame, int size, socket_info *soc_info); 

int __f_exec(char * const path);

#endif
