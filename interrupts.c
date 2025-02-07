#include <stdint.h>
#include "interrupts.h"

/*TI manual 6.2.1
 * Skiping Steps 1 and 2
 * Step 3
 * 	default all interrupts are IRQ and highest prioirity
 * 	we can change it to FRQ if we want using this step
 * 	for now we will keep all as default
 *
 * Step 4
 * 	by default all interrupts are masked
 * 	write to INTC_MIR_CLEARn to unmask
 * 	for now we will unmask all and send all to 
 * 	the default interrupt handler, which will
 * 	proccess request based on interrupt number
 * */
void init_interrupts(void){

    /* Unmasking all interrupts for now */

    *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR0) = 0xFFFFFFFF;

    *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR1) = 0xFFFFFFFF;

    *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR2) = 0xFFFFFFFF;

    *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR3) = 0xFFFFFFFF;
}
