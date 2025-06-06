/* CMPT432 - ImplementationTeam00 */

#ifndef UART_H
#define UART_H

#define UART0_BASE 0x44E09000
#define UART_SYSC 0x54
#define UART_SYSS 0x58
#define UART_LCR 0xC
#define UART_MCR 0x10
#define UART_FCR 0x8    /* Only OP and Mode A */
#define UART_SCR 0x40
#define UART_TCR 0x18
#define UART_EFR 0x8    /* Only Mode B */
#define UART_TLR 0x1C
#define UART_MDR1 0x20
#define UART_MDR3 0x80
#define UART_IER 0x4    /* Only OP mode */
#define UART_DLL 0x0
#define UART_DLH 0x4
#define UART_LSR 0x14

#define IER_UART 0x4

#define CM_BASE 0x44E10000
#define CONF_UART0_TXD 0x0974
#define CONF_UART0_RXD 0x0970


#define CKM_PER_BASE 0x44E00000
#define CKM_PER_L4HS_CLKSTCTRL 0x11C

#define CKM_WKUP_BASE 0x44E00400
#define CKM_WKUP_CLKSTCTRL 0x0
#define CKM_WKUP_UART0_CLKCTRL 0xB4


void uart0_init(void);
void uart0_putch(char c);
void uart0_puts(const char* str);
void uart0_putsln(const char* str);
char* itohex(uint32_t number, char* str);
void reverse(char* str);
char* itoa(int num, char* str);
void uart0_printHex(uint32_t number);
void uart0_printf(const char* str, ...);
char uart0_getch();
int uart0_poll();
char* uart0_fgets(char* str, int n, int stream);
/*void uart0_printFloat(float num);*/

#endif
