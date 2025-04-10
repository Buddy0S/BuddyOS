/* CMPT432 - ImplementationTeam00 */

#include <stdint.h>
#include "../arch/uart.h"
#include "../include/misc/reg.h"


int main(void) {
    uart0_puts("hi guys\n");
    return 0;
}

void uart0_putch(char c) {

    while((READ8(UART0_BASE + UART_LSR) & 0x20) != 0x20);

    WRITE8(UART0_BASE + 0x0, c);
}

void uart0_puts(const char* str) {
    while (*str != 0) {
         uart0_putch(*str);
         str++;
    }
}

