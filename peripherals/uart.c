#include "led.h"
#include <stdint.h>
#include "uart.h"

void uart0_init(void) {
    
    uint8_t reg_8;
    uint32_t reg_32;

    uint32_t div = (uint32_t)(48e6 / (16 * 115200)); /* 48 MHz / (16 * Baud Rate) */

    /* Init UART GPIO pins */
    *(volatile uint32_t*)(CM_BASE + CONF_UART0_TXD) = 0;
    *(volatile uint32_t*)(CM_BASE + CONF_UART0_RXD) = (1<<4)|(1<<5);
    *(volatile uint32_t*)(CM_BASE + CONF_UART0_TXD) &= ~(0x7);
    *(volatile uint32_t*)(CM_BASE + CONF_UART0_RXD) &= ~(0x7);

    reg_32 = *(volatile uint32_t*)(CKM_WKUP_BASE + CKM_WKUP_CLKSTCTRL);
    reg_32 &= ~(0x3);
    reg_32 |= 0x2;
    *(volatile uint32_t*)(CKM_WKUP_BASE + CKM_WKUP_CLKSTCTRL) = reg_32;

    /* Enable High speed peripheral clock */
    reg_32 = *(volatile uint32_t*)(CKM_PER_BASE + CKM_PER_L4HS_CLKSTCTRL);
    reg_32 &= ~(0x3);
    reg_32 |= 0x2;
    *(volatile uint32_t*)(CKM_PER_BASE + CKM_PER_L4HS_CLKSTCTRL) = reg_32;

    /* Enable UART0 Clk */
    reg_32 = *(volatile uint32_t*)(CKM_WKUP_BASE + CKM_WKUP_UART0_CLKCTRL);
    reg_32 &= ~(0x3);
    reg_32 |= 0x2;
    *(volatile uint32_t*)(CKM_WKUP_BASE + CKM_WKUP_UART0_CLKCTRL) = reg_32;
    while(*(volatile uint32_t*)(CKM_WKUP_BASE + CKM_WKUP_UART0_CLKCTRL) &
        (3<<16) != 0);

    /* Software reset UART 0 */
    *(volatile uint32_t*)(UART0_BASE + UART_SYSC) |= 0x2;
    while(*(volatile uint32_t*)(UART0_BASE + UART_SYSS) & 0x1 == 0);

    /* Turn off power saving mechanism (idle requests) */
    *(volatile uint8_t*)(UART0_BASE + UART_SYSC) |= (1<<3);

    /* Spin until txfifo is empty */
    while(*(volatile uint32_t*)(UART0_BASE + UART_LSR) & 0x40 != 0x40);

    /* Set Baud rate */
    *(volatile uint8_t*)(UART0_BASE + UART_MDR1) |= 0x7;
    *(volatile uint8_t*)(UART0_BASE + UART_LCR) = ~(0x7C);
    *(volatile uint8_t*)(UART0_BASE + UART_DLL) = div & 0XFF;
    *(volatile uint8_t*)(UART0_BASE + UART_DLH) = (div >> 8) & 0x3F;
    *(volatile uint8_t*)(UART0_BASE + UART_MCR) = 0x3;
    *(volatile uint8_t*)(UART0_BASE + UART_FCR) = 0x7;
    *(volatile uint8_t*)(UART0_BASE + UART_LCR) = 0x3;
    *(volatile uint8_t*)(UART0_BASE + UART_MDR1) &= ~(0x7);

    *(volatile uint32_t*)(UART0_BASE + IER_UART) = 0x1;
    *(volatile uint8_t*)(UART0_BASE + UART_MDR1) = 0x0;
    

}

void uart0_putch(char c) {

    while(*(volatile uint8_t*)(UART0_BASE + UART_LSR) & 0x20 != 0x20);

    *(volatile uint8_t*)((volatile char*)UART0_BASE + 0x0) = c;

}

/* NOT QUITE WORKING - Might have something to do with polling instead of
 * UART interrupts */
char uart0_getch() {

    //LEDoff(LED0); /* Uncommenting this line makes it work for some reason */

    while(*(volatile uint8_t*)(UART0_BASE + UART_LSR) & 0x1 == 0x0);

    return *(volatile uint8_t*)((volatile char*)UART0_BASE + 0x0);

}
