#include "led.h"
#include <stdint.h>
#include "uart.h"
#include "reg.h"

void uart0_init(void) {
    
    uint8_t reg_8;
    uint32_t reg_32;

    uint32_t div = (uint32_t)(48e6 / (16 * 115200)); /* 48 MHz / (16 * Baud Rate) */

    /* Init UART GPIO pins */
    WRITE32(CM_BASE + CONF_UART0_TXD, 0);
    WRITE32(CM_BASE + CONF_UART0_RXD, (1<<4)|(1<<5));
    WRITE32(CM_BASE + CONF_UART0_TXD, READ32(CM_BASE + CONF_UART0_TXD) & ~0x7);
    WRITE32(CM_BASE + CONF_UART0_RXD, READ32(CM_BASE + CONF_UART0_RXD) & ~0x7);

    reg_32 = READ32(CKM_WKUP_BASE + CKM_WKUP_CLKSTCTRL);
    reg_32 &= ~(0x3);
    reg_32 |= 0x2;
    WRITE32(CKM_WKUP_BASE + CKM_WKUP_CLKSTCTRL, reg_32);

    /* Enable High speed peripheral clock */
    reg_32 = READ32(CKM_PER_BASE + CKM_PER_L4HS_CLKSTCTRL);
    reg_32 &= ~(0x3);
    reg_32 |= 0x2;
    WRITE32(CKM_PER_BASE + CKM_PER_L4HS_CLKSTCTRL, reg_32);

    /* Enable UART0 Clk */
    reg_32 = READ32(CKM_WKUP_BASE + CKM_WKUP_UART0_CLKCTRL);
    reg_32 &= ~(0x3);
    reg_32 |= 0x2;
    WRITE32(CKM_WKUP_BASE + CKM_WKUP_UART0_CLKCTRL, reg_32);
    while(READ32(CKM_WKUP_BASE + CKM_WKUP_UART0_CLKCTRL) &
        (3<<16) != 0);

    /* Software reset UART 0 */
    WRITE32(UART0_BASE + UART_SYSC, READ32(UART0_BASE + UART_SYSC) | 0x2);
    while(READ32(UART0_BASE + UART_SYSS) & 0x1 == 0);

    /* Turn off power saving mechanism (idle requests) */
    WRITE8(UART0_BASE + UART_SYSC, READ8(UART0_BASE + UART_SYSC) | (1<<3));

    /* Spin until txfifo is empty */
    while(READ32(UART0_BASE + UART_LSR) & 0x40 != 0x40);

    /* Set Baud rate */
    WRITE8(UART0_BASE + UART_MDR1, READ8(UART0_BASE + UART_MDR1) | 0x7);
    WRITE8(UART0_BASE + UART_LCR, ~(0x7C));
    WRITE8(UART0_BASE + UART_DLL, div & 0XFF);
    WRITE8(UART0_BASE + UART_DLH, (div >> 8) & 0x3F);
    WRITE8(UART0_BASE + UART_MCR, 0x3);
    WRITE8(UART0_BASE + UART_FCR, 0x7);
    WRITE8(UART0_BASE + UART_LCR, 0x3);
    WRITE8(UART0_BASE + UART_MDR1, READ8(UART0_BASE + UART_MDR1) & ~(0x7));

    WRITE32(UART0_BASE + IER_UART, 0x1);
    WRITE8(UART0_BASE + UART_MDR1, 0x0);
    

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
    uart0_putch('\r');
}

void uart0_putsln(const char* str) {
    if (str) {
    	uart0_puts(str);
    }
    uart0_putch('\n');
}

/* NOT QUITE WORKING - Might have something to do with polling instead of
 * UART interrupts */
char uart0_getch() {

    //LEDoff(LED0); /* Uncommenting this line makes it work for some reason */

    while(*(volatile uint8_t*)(UART0_BASE + UART_LSR) & 0x1 == 0x0);

    return *(volatile uint8_t*)((volatile char*)UART0_BASE + 0x0);

}

void uart0_test() {
	uart0_putsln("Test 1: Testing single char below: ");
	const char testSingleChar = 'N';
	uart0_putch(testSingleChar);
	uart0_putsln("");

	uart0_putsln("Test 2: Testing new line below: ");
	const char testNewLine = '\n';	
	uart0_putch(testNewLine);
	uart0_putsln("");
	
	uart0_putsln("Test 3: Testing empty char below: ");
	const char testEmptyChar = '\0';
	uart0_putch(testEmptyChar);
	uart0_putsln("");

	uart0_putsln("Test 4: Testing empty string below: ");
	const char* testEmptyString = "";
	uart0_putsln(testEmptyString);
	uart0_putsln("");

	uart0_putsln("Test 5: Testing uart0_puts with manual \\n below: ");
	const char* testManualNewLine = "New line\n";
	uart0_puts(testManualNewLine);

	uart0_putsln("Test 6: Testing uart0_putsln below: ");
	const char* testAutoNewLine = "Buddy";
	uart0_putsln(testAutoNewLine);

	uart0_putsln("Test 7: Testing spaces below: ");
	const char* testSpaces = "We are so awesome";
	uart0_putsln(testSpaces);

	uart0_putsln ("Test 8: Testing null below: ");
        const char* testNull = 0;
	uart0_putsln(testNull);	
}
