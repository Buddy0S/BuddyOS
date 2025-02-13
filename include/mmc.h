#ifndef MMC_H
#define MMC_H

#define CONTROL_MODULE 0x44E10000
#define CM_PER_BASE 0x44E00000
#define MMC_BASE 0x48060000

/* pins for mmc */
/* TI manual 9.3.1 */
/* TI manual 9.3.1.50  has pin register feilds */
#define MMC_CMD (CONTROL_MODULE + 0x904)
#define MMC_CLK (CONTROL_MODULE + 0x900)
#define MMC_DAT0 (CONTROL_MODULE + 0x8FC)
#define MMC_DAT1 (CONTROL_MODULE + 0x8F8)
#define MMC_DAT2 (CONTROL_MODULE + 0x8F4)
#define MMC_DAT3 (CONTROL_MODULE + 0x8F0)

/* mmc clock control register */
/* TI manual 8.1.12.1.13 */
/* first 2 bits are for module mode */
/* set to 0x2 for enable */
#define MMC_CLKCTRL (CM_PER_BASE + 0x3C)

/* MMC Registers TI manual 18.5.1 */

/* TI manual 18.5.1.1 */
/* Set bit 1 to 1 for Soft reset */
/* Wake up config 
 *
 * set bit 2 to enable wake up capability
 *
 * bit 4-3 set to 0x2
 *
 * bit  0 to 1 prob a good idea to set
 *
 * */
#define SD_SYSCONFIG (MMC_BASE + 0x110)

/* TI manual 18.5.1.2 */
/* bit 0 when set to 1 means reset is done */
#define SD_SYSSTATUS (MMC_BASE + 0x114)

/* TI manual 18.5.1.23 */
/* reading this section 
 * the only thing we need to set is the voltages
 * everything else is good at default
 * need to set bits 24 25 26 to 1
 * */
#define SD_CAPA (MMC_BASE + 0x240)

/* TI manual 18.5.1.17 */
/* gonna need to set bit 24 to enabled
 * so that card wakes up on interrupt 
 *
 * host bus config
 * SDBP power off first
 * set bit 8 to 0
 * select voltage
 * set bit 11-9 to 0x7 3.3v 
 * DTW bit 1 to 0
 * power on
 * set bit 8 to 1
 *
 * */
#define SD_HCTL (MMC_BASE + 0x228)

/* TI manual 18.5.1.5 */
/* says to write to specfic bits to config data and command transfer 
 * need to set DW8 bit 5 to 0
 * CEATA bit 12 should also be set to 0
 * */
#define SD_CON (MMC_BASE + 0x12C)

/* TI manual 18.5.1.18 */
/*
 * Enable internal clock set bit 0 to 1
 * set the clock frequency bit 15-6
 *  - clear it 
 *  - need it to be less then = to 80kHz
 *  - so its refrence clock / value in bit 15-6
 *  - refrence clock = 96Mhz
 *  - we want it to be slow but not super slow for init sequence
 *  - max is 1024 so lets use 512
 *
 * enable external clock by seting bit 2 to  1
 *
 *
 * */
#define SD_SYSCTL (MMC_BASE + 0x22C)

/* TI manual 18.5.1.20 */
/* Just gonna enable all interrupts */
#define SD_IE (MMC_BASE + 0x234)

void initMMC();

#endif
