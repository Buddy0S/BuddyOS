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
 *  - max divisor is 1023 which gives us a freq of 93khz
 *  - bro how do we get down to 80khz when the max divisor is 1023
 *  - im just gonna start at 0x200 and go up until it works
 *
 * enable external clock by seting bit 2 to  1
 *
 *
 * */
#define SD_SYSCTL (MMC_BASE + 0x22C)

/* TI manual 18.5.1.20 */
/* Just gonna enable all interrupts */
#define SD_IE (MMC_BASE + 0x234)

/* TI manual 18.5.1.10 */
/* this register is kinda crazy, conrtols what commands we send 
 * to the sd card 
 *
 * bit 29-24 command 
 *
 * bit 17-16 response type
 *
 * first 6 bits are for flags
 *
 * */
#define SD_CMD (MMC_BASE + 0x20C)

/* TI manual 18.5.1.19 */
/* can use this to check status of interrupts */
/* bit 0 can be read to check if a command has completed 
 * set to 1 if cmd has been completed
 * */
#define SD_STAT (MMC_BASE + 0x230)

/* TI manual 18.5.1.16 */
/* can use to get status of host controller 
 * bit 16 CINS can be read to see if card is inserted
 * */
#define SD_PSTATE (MMC_BASE + 0x224)

/* TI manual 18.5.1.9 */
/* uses all 32 bits for argument for sd command */
#define SD_ARG (MMC_BASE + 0x206)

void initMMC();
int detectSDcard();
void idCard();

#endif
