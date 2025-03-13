#ifndef SYSCALL_H
#define SYSCALL_H

#define TEST_SYSCALL_2_ARGS_NR 0

#define STR(x) #x

#define SYSCALL(x)                          \
({                                           \
    register int return_val asm("r0");      \
    asm volatile (" svc #" STR(x) "  \n\t");\
    return_val;                             \
})                                           \


#endif
