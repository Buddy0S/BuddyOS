#include <stdint.h>
#include "memory.h"
#include "uart.h"


/* yield */
void yield(void) {}

/* fork */
int fork(void) {}

/* malloc (temp) */
void *malloc(size_t size) {
    return kmalloc(size);
}

/* free */
void free(void *addr) {
    kfree(addr);
}

/* get pid */
uint32_t *getPid() {}

/* kill */
int kill(uint32_t pid);

/* printf */
//void printf(void *in) {}
