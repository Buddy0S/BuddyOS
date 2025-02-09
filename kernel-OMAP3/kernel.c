#include "../include/memory/qemu-mem.h"

/* Define UART1 transmit register address (UART1 + offset) */

/* we are using uart1 because omap35x does not have uart0 lol
   also we are definitley not setting it up correctly */
#define UART1_THR (UART1 + UART_THR)

void kernel_main(void) {
    volatile unsigned int *uart_thr = (unsigned int *)UART1_THR;
    while (1) {
        *uart_thr = 'p';
    }
}
