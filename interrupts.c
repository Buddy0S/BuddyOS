#include <stdint.h>
#include "interrupts.h"
#include "led.h"

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

    /* soft reset INTC*/
    *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_SYSCONFIG) |= 0x2;

    /* spin until reset finished */
    while (!((*(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_SYSSTATUS)) & 0x1)){}

   
    /* Unmasking interrupts that we need */

    //*(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR0) = 0xFFFFFFFF;

    //*(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR1) = 0xFFFFFFFF;

    //*(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR2) = 0xFFFFFFFF;

    //*(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR3) = 0xFFFFFFFF;
}


void interrupt_handler(){

  //LEDon(LED1);

}

/* Renables interrupts by clearing the 
 * IRQ and FIQ disable bits in cpsr
 * */
void enable_interrupts(void){

    asm(" mrs r1, cpsr   \n\t"
        " bic r1, #0xC0  \n\t"
        " msr cpsr, r1 \n\t");
    
}

/* disables interrupts by seting the
 * IRQ and FIQ disable bits in cpsr
 * */
void disable_interrupts(void){

    asm(" mrs r1, cpsr   \n\t"
        " orr r1, #0xC0  \n\t"
        " msr cpsr, r1 \n\t");

}
