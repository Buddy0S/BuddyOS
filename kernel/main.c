#include <stdint.h>
#include "uart.h"
#include "led.h"
#include "reg.h"

int main(){

    WRITE8(UART0_BASE, (uint8_t)'H');
    WRITE8(UART0_BASE, (uint8_t)'I');
    WRITE8(UART0_BASE, (uint8_t)' ');
    WRITE8(UART0_BASE, (uint8_t)'F');
    WRITE8(UART0_BASE, (uint8_t)'R');
    WRITE8(UART0_BASE, (uint8_t)'O');
    WRITE8(UART0_BASE, (uint8_t)'M');
    WRITE8(UART0_BASE, (uint8_t)' ');
    WRITE8(UART0_BASE, (uint8_t)'K');
    WRITE8(UART0_BASE, (uint8_t)'E');
    WRITE8(UART0_BASE, (uint8_t)'R');
    WRITE8(UART0_BASE, (uint8_t)'N');
    WRITE8(UART0_BASE, (uint8_t)'E');
    WRITE8(UART0_BASE, (uint8_t)'L');
    WRITE8(UART0_BASE, (uint8_t)'\n');


    while(1){}	

    return 0;
}
