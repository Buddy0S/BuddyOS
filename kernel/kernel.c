#include "../include/memory/qemu-mem.h"

/* Define UART1 transmit register address (UART1 + offset) */

/* we are using uart1 because omap35x does not have uart0 lol
   also we are definitley not setting it up correctly */
#define UART1_THR (UART1 + UART_THR)

/* Function to send one character over UART1 */
void uart_send(char c) {
    volatile unsigned int *uart_thr = (unsigned int *)UART1_THR;
    /* In a full implementation, we're gonna have to check the UART status register here. */
    *uart_thr = c;
}

/* Send a string over UART0. */
void uart_print(const char *str) {
    while (*str) {
        uart_send(*str++);
    }
}

void kernel_main(void) {
    uart_print("Kernel working\n");
    /* Loop forever to keep the kernel running */
    while (1) { }
}

/* _start is the entry point that our linker script gonna reference */
void _start(void) {
    kernel_main();
} 
