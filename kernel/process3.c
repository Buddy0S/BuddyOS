#include "uart.h"

extern void delay(void);
extern void yield(void);

void process3(void) {
    while (1) {
        uart_puts("Process 3\n");
        delay();
        yield();
    }
}

