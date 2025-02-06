
#include <stdint.h>
#define UART0_OFFSET 0
#define UART_SYSC 0
#define UART_SYSS 0
#define UART_LCR 0
#define UART_MCR 0
#define UART_FCR 0
#define UART_SCR 0
#define UART_EFR 0
#define UART_TLR 0
#define UART_MDR1 0
#define UART_IER 0
#define UART_DLL 0
#define UART_DLH 0

void uart0_init(void) {

    volatile uint32_t old_lcr;
    volatile uint32_t enhanced_en_bit;
    volatile uint32_t tcr_tlr_bit;

    /* all of this from 19.4.1.1 */

    /* soft reset uart */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_SYSC) |= 0x2;

    /* spin until reset finished */
    while (((*(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_SYSS)) & 0x1)
            != 0x1);

    /* save lcr value then set to 0x00BF to enter config b mode */
    old_lcr = *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_LCR);
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_LCR) = 0x00BF;

    /* save en bit then set it to 1 */
    enhanced_en_bit = *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_EFR) & (0x1 << 4);
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_EFR) |= (0x1 << 4);

    /* enter config mode a */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_LCR) = 0x0080;

    /* set tcr_tlr bit to gain access to fcr register */
    tcr_tlr_bit = *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_MCR) & (0x1 << 6);
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_MCR) |= (0x1 << 6);

    /* put wanted values into fcr register */
    /* 0x00000001 means disable dma, enable fifo and use minimum interrupt trigger thresholds */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_FCR) = 0x00000001;

    /* reenter config mode b */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_LCR) = 0x00BF;

    /* put wanted values into tlr register */
    /* all 0s means disable dma */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_TLR) = 0x00000000;

    /* put wanted values into fcr register */
    /* 0x00000001 means disable dma, and use default granularity for fifo triggers */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_SCR) = 0x00000001;

    /* restore saved en bit */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_EFR) &= ~enhanced_en_bit;
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_EFR) |= enhanced_en_bit;

    /* reenter config mode a */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_LCR) = 0x0080;

    /* restore other saved fields */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_MCR) &= ~tcr_tlr_bit;
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_MCR) |= tcr_tlr_bit;
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_LCR) = old_lcr;

    /* finished initial setup of uart */
    /* next is programming the baud rate and more interrupt settings */

    /* disable uart temporarily */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_MDR1) |= 0x7;

    /* enter config mode b */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_LCR) = 0x00BF;

    /* save en bit then set it to 1 */
    enhanced_en_bit = *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_EFR) & (0x1 << 4);
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_EFR) |= (0x1 << 4);

    /* enter operational mode */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_LCR) = 0x0000;

    /* clear ier register */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_IER) = 0x0000;

    /* enter config mode b */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_LCR) = 0x00BF;

    /* give divisor values for baud rate */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_DLL) = 0x1A;
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_DLH) = 0x00;

    /* reenter operational mode */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_LCR) = 0x0000;

    /* enable interrupt flags in ier register */
    /* for now just gonna use 0x0000  for polling */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_IER) = 0x0000;

    /* reenter config mode b */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_LCR) = 0x00BF;

    /* restore saved en bit */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_EFR) &= ~enhanced_en_bit;
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_EFR) |= enhanced_en_bit;

    /* load protocol formatting */
    *(volatile uint32_t*)((volatile char*)UART0_OFFSET + UART_LCR) &= ~(0x3 << 6);






}
