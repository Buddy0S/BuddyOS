#include "../include/memory/qemu-mem.h"

#define UART0_DR (UART0 + UART_DR)

void uart_putc(char c) {
    volatile unsigned int *uart_dr = (unsigned int *)(UART0_DR);
    *uart_dr = c;
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

void kernel_main() {
    uart_puts("welcome to buddyOS\n");
    while (1);
}

