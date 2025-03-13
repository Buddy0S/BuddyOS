#include <stdint.h>
#include "uart.h"


/* yield */
void yield(void) {}

/* fork */
int fork(void) {}

/* malloc */
void *malloc(size_t size) {}

/* get pid */
uint32_t *getPid() {}

/* kill */
int kill(uint32_t pid);

/* printf */
//void printf(void *in) {}
