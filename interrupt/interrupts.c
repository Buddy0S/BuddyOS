#include <stdint.h>
#include "interrupts.h"
#include "led.h"
#include "timer.h"
#include "uart.h"

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


    /* timer0 interrupt*/
   *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR2) = (0x1 << 2);

    /* mmc interrupts*/
   *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR2) = (0x1);

    /* UART0 interrupt */
   *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR2) = (0x1 << 8);

   // *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR2) = 0xFFFFFFFF;

   // *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_MIR_CLEAR3) = 0xFFFFFFFF;
}

int ledmode = 0;

void timer_isr(){

    volatile int i;
    volatile int T = 500000;

    if (ledmode == 0){
        LEDon(LED0);

        LEDon(LED1);

        LEDon(LED2);

        LEDon(LED3);

	ledmode = 1;
	
    }

    else if (ledmode == 1){
        LEDoff(LED0);

        LEDoff(LED1);

        LEDoff(LED2);

        LEDoff(LED3);

	ledmode = 0;
    }

   

    

}

void UART0_isr(){

    char rec = *(volatile uint8_t*)UART0_BASE;

    uart0_putch(rec);
}

void interrupt_handler(){

    /* get interrupt number */
    volatile uint32_t irqnum = *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_IRQ) & 0x7F; 


    //uart0_printf("IRQ number %d\n", (int) irqnum);

    /* TIMER 0 interrrupt*/
    if (irqnum == 66){
   
	    
        *(volatile uint32_t*)((volatile char*)DMTIMER0_BASE + DMTIMER0_IRQ_STATUS) = 0x2;

        *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_ISR_CLEAR2) = (0x1 << 2);	

        timer_isr();

        *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_CONTROL) = 0x1;

    
    }

    /* UART 0 interrupt*/
    if (irqnum == 72){
    
       *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_ISR_CLEAR2) = (0x1 << 8); 

       UART0_isr();       

       *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_CONTROL) = 0x1;
	
    }

}

void svc_handler(uint32_t svc_num, uint32_t args[]) {

    uart0_printf("%d", svc_num);
    return;

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

