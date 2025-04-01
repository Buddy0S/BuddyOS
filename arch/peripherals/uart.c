#include "led.h"
#include <stdint.h>
#include "args.h"
#include "uart.h"
#include "reg.h"

#define ENTER		13
#define BACKSPACE	8

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

/*void uart0_printFloat(float num) {
	int intNum = (int)num;
	float fractionalNum = num - intNum;

	uart0_printitoa(intNum);
	uart0_putch('.');
	
	while (fractionalNum > 0.00001) {
		fractionalNum *= BASE10;
		int digit = (int)fractionalNum;
		uart0_putch(digit + '0');
		fractionalNum -= digit;
	}
	return;
}*/

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
				/*case 'f': {
					float formattedFloat = va_args(args, float);
					uart0_printFloat(formattedFloat);
					break;
				}*/
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

    /* LEDoff(LED0); Uncommenting this line makes it work for some reason */

    while((*(volatile uint8_t*)(UART0_BASE + UART_LSR) & 0x1) == 0x0);

    return *(volatile uint8_t*)((volatile char*)UART0_BASE + 0x0);

}

char* uart0_fgets(char* str, int n, int stream){

  char userchar = '\0';
  int charindex = 0;

  while (userchar != ENTER && charindex < n - 1){
    userchar = uart0_getch();
    if (userchar == BACKSPACE && charindex > 0){
      charindex--;
    }else if (userchar != ENTER){
      str[charindex] = userchar;
      charindex++;
    }
  }

  str[charindex] = '\0';

  return str;

}



