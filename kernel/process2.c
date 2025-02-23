#include "uart.h"

extern void delay(void);
extern void yield(void);

void process2(void) {
    while (1) {
        uart_puts("Process 2\n");
        delay();
        yield();
    }
}

