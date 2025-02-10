#include "uart.h"

void kernel_main() {
    uart_puts("welcome to buddyOS\n");
    while (1);
}

