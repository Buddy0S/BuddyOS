#include <stdint.h>
#include "memory.h"
#include "uart.h"

extern PCB pcb;
extern int current_index;

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
uint32_t *getPid(int current_index) {
    return pcb[current_index]->pid;
}

/* kill */
int kill(uint32_t pid);

/* printf */
//void printf(void *in) {}
