#pragma once

#define DMTIMER0_BASE 0x44E05000

#define DMTIMER0_TLDR 0x40
#define DMTIMER0_TWPS 0x48
#define DMTIMER0_TTGR 0x44
#define DMTIMER0_TCLR 0x38
#define DMTIMER0_IRQ_ENABLE 0x2C
#define DMTIMER0_IRQ_STATUS 0x28

/* TI manual 8.1.12.2 */
#define CM_WKUP_BASE 0x44E00400 /* Clock module wake up registers */

#define CM_WKUP_CONTROL 0x4
#define CM_WKUP_TIMER0 0x10

void initTimer();
