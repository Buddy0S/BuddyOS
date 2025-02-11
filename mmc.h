#ifndef MMC_H
#define MMC_H

#define CONTROL_MODULE 0x44E10000

/* pins for mmc */
/* TI manual 9.3.1 */

#define MMC_CMD (CONTROL_MODULE + 0x904)
#define MMC_CLK (CONTROL_MODULE + 0x900)
#define MMC_DAT0 (CONTROL_MODULE + 0x8FC)
#define MMC_DAT1 (CONTROL_MODULE + 0x8F8)
#define MMC_DAT2 (CONTROL_MODULE + 0x8F4)
#define MMC_DAT3 (CONTROL_MODULE + 0x8F0)

#endif
