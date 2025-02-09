#include "../include/memory/qemu-mem.h"

/* Define UART1 transmit register address (UART1 + offset) */

/* we are using uart1 because omap35x does not have uart0 lol
   also we are definitley not setting it up correctly */
#define UART1_THR (UART1 + UART_THR)
#define UART1_LCR (UART1 + UART_LCR)
#define UART1_MCR (UART1 + UART_MCR)
#define UART1_LSR (UART1 + UART_LSR)

/* initialize some uart stuff */
void uart_init(void) {
    volatile unsigned int *uart_lcr = (unsigned int *)UART1_LCR;
    volatile unsigned int *uart_mcr = (unsigned int *)UART1_MCR;

    /* 8 bit data */
    *uart_lcr = 0x3;

    /* enable tranmitter and receiver */
    *uart_mcr = 0x3;


}

int uart_is_transmitter_ready(void) {
    volatile unsigned int *uart_lsr = (unsigned int *)UART1_LSR;
    
    /* check if uart thr is empty */
    return (*uart_lsr & 0x20) != 0;
}

/* Function to send one character over UART1 */
void uart_send(char c) {
    volatile unsigned int *uart_thr = (unsigned int *)UART1_THR;
    while (!uart_is_transmitter_ready()) {}

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
    while (1) {}
}

/* _start is the entry point that our linker script gonna reference */
void _start(void) {
    kernel_main();
} 
