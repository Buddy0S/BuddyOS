#ifndef SYSCALL_H
#define SYSCALL_H

#define STR(x) #x

#define SYSCALL(x)                          \
({                                          \
    register int return_val asm("r0");      \
    asm volatile (" svc #" STR(x) "  \n\t");    \
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

#endif
