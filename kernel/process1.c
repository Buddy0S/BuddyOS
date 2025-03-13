#include "uart.h"

/* Declare external helper routines from kernel */
extern void delay(void);
extern void yield(void);

void process1(void) {
    while (1) {
        uart_puts("Process 1\n");
        yield();
    }
}

