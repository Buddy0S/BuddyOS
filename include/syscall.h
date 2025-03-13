#ifndef SYSCALL_H
#define SYSCALL_H

#define SYSCALL(x)                          \
({                                          \
    register int return_val asm("r0");      \
    asm volatile (" svc #" #x "  \n\t");    \
    return_val;                             \
})                                          \

#define SYSCALL_TEST_2_ARGS_NR  0
#define SYSCALL_YIELD_NR        1
#define SYSCALL_MALLOC_NR       2
#define SYSCALL_FORK_NR         3
#define SYSCALL_KILL_NR         4
#define SYSCALL_GETPID_NR       5

#endif
