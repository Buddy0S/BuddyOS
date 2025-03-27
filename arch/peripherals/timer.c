#include "timer.h"
#include <stdint.h>
#include "reg.h"


/* Ti 8.1.12.1 has info on all the registers */
void initTimer() {
    /* Enable functional clock for Timer0  */
    WRITE32(CM_WKUP_BASE + CM_WKUP_CONTROL, 0x2);

    /* Wait until Timer0 module is fully enabled */
    while (READ32(CM_WKUP_BASE + CM_WKUP_TIMER0) & (0x3 << 16)) {}

    /* Ti 20.1.3.6 */
    /* Configure the Timer Load Register (TLDR) for desired interrupt frequency */
    WRITE32(DMTIMER0_BASE + DMTIMER0_TLDR, 0xFFFF8000);
    /* 0xFFFF8000 = 1s period, 0xFFFFFEBF = 10 ms? */

    /* Wait for the write to complete 
     * TWPS ensures register update is done
     */
    while (READ32(DMTIMER0_BASE + DMTIMER0_TWPS) & (0x4)) {}

    /* Set Timer Trigger Register (TTGR) to force a reload of TLDR 
     */
    WRITE32(DMTIMER0_BASE + DMTIMER0_TTGR, 0x1);

    /* Wait for write completion */
    while (READ32(DMTIMER0_BASE + DMTIMER0_TWPS) & (0x8)) {}

    /* Configure Timer Control Register (TCLR) 
     * Set Auto-reload mode so the timer reloads after reaching zero
     */
    WRITE32(DMTIMER0_BASE + DMTIMER0_TCLR, READ32(DMTIMER0_BASE + DMTIMER0_TCLR) | 0x2);

    /* Wait for write completion */
    while (READ32(DMTIMER0_BASE + DMTIMER0_TWPS) & (0x1)) {}

    /* Enable Timer Interrupt (IRQ Enable Register) */
    WRITE32(DMTIMER0_BASE + DMTIMER0_IRQ_ENABLE, READ32(DMTIMER0_BASE + DMTIMER0_IRQ_ENABLE) | 0x2);

    /* Start the Timer by setting the Start/Stop bit (bit 0) in TCLR */
    WRITE32(DMTIMER0_BASE + DMTIMER0_TCLR, READ32(DMTIMER0_BASE + DMTIMER0_TCLR) | 0x1);

    /* Wait for write completion */
    while (READ32(DMTIMER0_BASE + DMTIMER0_TWPS) & (0x1)) {}
}
