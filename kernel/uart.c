#include "qemu/uart.h"

void uart_putc(char c) {
    volatile unsigned int *uart_dr = (unsigned int *)(UART0 + UART_DR);
    *uart_dr = c;
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

