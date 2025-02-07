/* Special memory addresses */

#ifndef MEMORY_H
#define MEMORY_H

/* Clock Module bases */
#define CM_PER 0x44E00000
#define CM_WKUP 0x44E00400
#define CM_DPLL 0x44E00500
#define CM_MPU 0x44E00600
#define CM_DEVICE 0x44E00700
#define CM_RTC 0x44E00800
#define CM_GFX 0x44E00900
#define CM_CEFUSE 0x44E00A00

/* Power Reset Module bases */
#define PRM_IRQ 0x44E00B00
#define PRM_PER 0x44E00C00
#define PRM_WKUP 0x44E00D00
#define PRM_MPU 0x44E00E00
#define PRM_DEV 0x44E00F00
#define PRM_RTC 0x44E01000
#define PRM_GFX 0x44E01100
#define PRM_CEFUSE 0x44E01200

/* base UARTs */
#define UART0 0x44E09000
#define UART1 0x48022000
#define UART2 0x48024000
#define UART3 0x481A6000
#define UART4 0x481A8000
#define UART5 0x481AA000

/* UART offsets */
#define UART_THR 0x0 /* transmit */
#define UART_RHR 0x0 /* receive */
#define UART_DLL 0x0 /* divisor latch */
#define IER_UART 0x4 /* UART interrupt enable reg */


#endif
