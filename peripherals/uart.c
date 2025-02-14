#include "led.h"
#include <stdint.h>
#include "args.h"
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
}

void uart0_putsln(const char* str) {
    if (str) {
    	uart0_puts(str);
    }
    uart0_puts("\r\n");
}

char* itohex(uint32_t number, char* str) {
	int bitShift;
	
	int i = 0;
	char hexValues[] = "0123456789ABCDEF";
	str[i++] = '0';
	str[i++] = 'x';

	for (bitShift = 28; bitShift >= 0; bitShift -=4) {
		str[i++] = hexValues[(number >> bitShift) & 0xF];
	}

	str[i] = '\0';
	return str;
}

void uart0_printHex(uint32_t number) {
	int bitShift;
	char hexValues[] = "0123456789ABCDEF";
	uart0_puts("0x");
	
	for (bitShift = 28; bitShift >= 0; bitShift -=4) {
		uart0_putch(hexValues[(number >> bitShift) & 0xF]);
	}
}

void reverse(char* str) {
	int i, j;
	char c;

	for (i = 0, j = sizeof(str)-2; i<j; i++, j--) {
		c = str[i];
		str[i] = str[j];
		str[j] = c;	
	} 
}	

char* itoa(int num, char* str) {
	int i = 0;
	int isNegative = 0;

	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;	
	}

	if (num < 0) {
		isNegative = 1;
		num = -num;
	}

	while (num != 0) {
		int remainder = num % BASE10;
		str[i++] = num % BASE10 + '0';
		num /= BASE10;	
	}	

	if (isNegative) {
		str[i++] = '-';
	}

	str[i] = '\0';
	
	reverse(str);

	return str;
}

void uart0_printitoa(int num) {
	int isNegative = 0;
	int index = 0;
	char buf[12];

	if (num == 0) {
		uart0_putch('0');
		uart0_putch('\0');
		return;	
	}

	if (num < 0) {
		isNegative = 1;
		num = -num;
	}

	if (isNegative) {
		uart0_putch('-');
	}

	while (num > 0) {
		buf[index++] = num % BASE10 + '0';
		num /= BASE10;	
	}

	for (int i = index - 1; i >= 0; i--) {
		uart0_putch(buf[i]);
	}
	return;	
}

void uart0_printf(const char* str, ...) {
	va_list args;
	va_start(args, str);

	while (*str != 0) {
		if (*str == '%') {
			str++;
			switch(*str) {
				case 'c': {
					char formattedChar = (char) va_args(args, int);
					uart0_putch(formattedChar);
					break;
				}
				case 'd': {
					int formattedInt = va_args(args, int);
					uart0_printitoa(formattedInt);
					break;
				}
				case 'f': {
					double formattedDouble = va_args(args, double);
					//floatfunc
					break;
				}
				case 'p': {
					void* formattedPtr = va_args(args, void *);
					uart0_printHex((uint32_t)formattedPtr);
					break;
				}
				case 's': {
					const char* formattedString = va_args(args, char*);
					uart0_puts(formattedString);
					break;
				}
				case 'x': {
					uint32_t formattedHex = va_args(args, uint32_t);
					uart0_printHex(formattedHex);
					break;
				}
				case '%': {
					uart0_putch('%');
					break;
				}
				default: {
					uart0_putch('?');
					break;
				}
			}
		} else if (*str == '\n') {
			uart0_putch(*str);
			uart0_putch('\r');	
		} else {
			uart0_putch(*str);
		}
		str++;
	}
	va_end(args);
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
	const char* testManualNewLine = "New line\r\n";
	uart0_puts(testManualNewLine);

	uart0_putsln("Test 6: Testing uart0_putsln below: ");
	const char* testAutoNewLine = "Buddy";
	uart0_putsln(testAutoNewLine);

	uart0_putsln("Test 7: Testing spaces below: ");
	const char* testSpaces = "We are so awesome";
	uart0_putsln(testSpaces);

	uart0_putsln("Test 8: Testing null below: ");
        const char* testNull = 0;
	uart0_putsln(testNull);

	uart0_putsln("Test 9: Testing printf format specifiers below: ");
	const char testChar = 'B';
	int testInt = 42;
	const char* testString = "Nuts";
	uint32_t testHex = 3735928559;
	uint32_t testHex2 = 15;
	int testInt2 = 0;
	int testInt3 = -69;
	uart0_printf("Expected testString = Nuts, Actual testString = %s\nExpected testHex = 0xDEADBEEF, Actual testHex = %x\nExpected testChar = B, Actual testChar = %c\nExpected testHex2 = 0x0000000F, Actual testHex2 = %x\nExpected testInt = 42, Actual testInt = %d\nExpected testInt2 = 0, Actual testInt2 = %d\nExpected testInt3 = -69, Actual testInt3 = %d\n", testString, testHex, testChar, testHex2, testInt, testInt2, testInt3);
	//uart0_printf("Expected testChar = B, Actual testChar = %c\nExpected testString = Nuts, Actual testString = %s\nExpected testHex = 0xDEADBEEF, Actual = testHex = %x\n", testChar, testString, testHex); 
}
