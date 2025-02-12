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
#define SD_SYSCONFIG (MMC_BASE + 0x110)

/* TI manual 18.5.1.2 */
/* bit 0 when set to 1 means reset is done */
#define SD_SYSSTATUS (MMC_BASE + 0x114)

void initMMC();

#endif
