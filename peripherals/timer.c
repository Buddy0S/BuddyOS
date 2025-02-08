#ifndef TIMER_H 
#define TIMER_H

#include "timer.h"
#include <stdint.h>

void initTimer(){

    *(volatile uint32_t*)((volatile char*)CM_WKUP_BASE + CM_WKUP_CONTROL) = 0x2;

   while (*(volatile uint32_t*)((volatile char*)CM_WKUP_BASE + CM_WKUP_TIMER0) & (0x3 << 16)) {}

   /* Setting interrupt frequency TI manual 20.1.3.6 */

   *(volatile uint32_t*)((volatile char*)DMTIMER0_BASE + DMTIMER0_TLDR) = 0xFFFF8000;

   while (*(volatile uint32_t*)((volatile char*)DMTIMER0_BASE + DMTIMER0_TWPS) & (0x4)) {}

   *(volatile uint32_t*)((volatile char*)DMTIMER0_BASE + DMTIMER0_TTGR) = 0x1;

   while (*(volatile uint32_t*)((volatile char*)DMTIMER0_BASE + DMTIMER0_TWPS) & (0x8)) {}

   *(volatile uint32_t*)((volatile char*)DMTIMER0_BASE + DMTIMER0_TCLR) |= 0x2;

   while (*(volatile uint32_t*)((volatile char*)DMTIMER0_BASE + DMTIMER0_TWPS) & (0x1)) {}

   /* Enable interrupt */
   /* when interrupt is enabled i think execution HALTs, need to fix this*/
   *(volatile uint32_t*)((volatile char*)DMTIMER0_BASE + DMTIMER0_IRQ_ENABLE) |= 0x2;

   *(volatile uint32_t*)((volatile char*)DMTIMER0_BASE + DMTIMER0_TCLR) |= 0x1;

   while (*(volatile uint32_t*)((volatile char*)DMTIMER0_BASE + DMTIMER0_TWPS) & (0x1)) {}

}

#endif
