#define UART0 0x10009000  // Base address of UART0 for realview-pb-a8

void uart_putc(char c) {
    volatile unsigned int *UART0_DR = (unsigned int *)(UART0);
    *UART0_DR = c;
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

void kernel_main() {
    uart_puts("Sup bro\n");
    while (1);
}

