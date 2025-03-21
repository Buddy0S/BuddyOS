#include "proc.h"
#include "interrupts.h"
#include "led.h"
#include "timer.h"
#include "reg.h"
#include "uart.h"
#include <stdint.h>

uint32_t timer_counter = 1000;

uint32_t test_syscall(int a, int b) {
    return a + b;
}

void testprint(int a, int b) {
    uart0_printf("hi guys %x %x\n", a, b);
}

/* called by supervisor vector in idt when svc interrupt is raised.
 *
 * supervisor vector in vector_table.S passes syscall number and function
 * arguments to this handler.
 *
 * DO NOT ADD ANY LOCAL VARIABLES TO THIS FUNCTION BECAUSE IT WILL OBLITERATE
 * THE SVC STACK
 * */
void svc_handler(uint32_t svc_num, uint32_t args[]) {
    // arguments will be put into the args array, but technically its just
    // a pointer to the bottom of the process stack that performed the syscall
    current_process->r_args = args;
    current_process->status = svc_num;
    current_process->trap_reason = SYSCALL;
    switch_to_dispatch(current_process, kernel_process);
}

void isr_switch(uint32_t isr_num) {
    current_process->status = isr_num;
    current_process->trap_reason = INTERRUPT;
    switch_to_dispatch(current_process, kernel_process);
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

/* this function somehow doesnt break irq stack frame, should probably cut
 * it down, but if i move the whole thing into the dispatcher, the timer
 * irq goes off 1000 times per second and obliterates everything with like 5
 * different context switches per timer interrupt */
void interrupt_handler() {

    /* get interrupt number */
    volatile uint32_t irqnum = *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_IRQ) & 0x7F; 

    static uint32_t seconds = 0;

    //if (irqnum != 66) uart0_printf("IRQ number %d\n", (int) irqnum);

    /* TIMER 0 interrrupt*/
    if (irqnum == 66) {

        /* need to have some quantum variable inside current_process that gets 
         * checked so that we know if we should go to dispatcher and reschedule
         * because i dont want to waste a lot of time on context switching
         * on timer interrupts that arent even going to make us re schedule */
        timer_counter -= 1;


        *(volatile uint32_t*)((volatile char*)DMTIMER0_BASE + DMTIMER0_IRQ_STATUS) = 0x2;

        *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_ISR_CLEAR2) = (0x1 << 2);	

        if (timer_counter <= 0) {
            timer_isr();
            //uart0_printf("%d seconds passed\n", ++seconds);
            timer_counter = 1000;
        }

        current_process->cpu_time -= 1;
        if (current_process->cpu_time <= 0) {
            //uart0_printf("time to switch\n");
            /* jump back to the dispatcher */
            current_process->quantum_elapsed = true;
            isr_switch(irqnum);
        }

        *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_CONTROL) = 0x1;
    }

    /* UART 0 interrupt*/
    if (irqnum == 72){

        *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_ISR_CLEAR2) = (0x1 << 8); 

        // when processes start blocking on stdin, gonna need to make this 
        // jump to dispatcher just like timer does above
        UART0_isr();       

        *(volatile uint32_t*)((volatile char*)INTERRUPTC_BASE + INTC_CONTROL) = 0x1;

    }

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

void kexception_handler(uint32_t exception, uint32_t pc) {
    uart0_printf("Exception Occurred: ");

    switch (exception) {
        case 1:  
            uart0_printf("Supervisor Call Exception\n");
            break;
        case 2:  
            {
                uint32_t addr; 
                uint32_t status;
                uint32_t reason;

                uart0_printf("Data Abort Exception\n");

                // this whole function is causing some embbeded bs


                asm volatile ("mrc p15, 0, %0, c6, c0, 0" : "=r" (addr));
                asm volatile ("mrc p15, 0, %0, c5, c0, 0" : "=r" (status));
                uart0_printf("Addr: %x \n", addr);
                uart0_printf("Status: %x \n", status);
                uart0_printf("Exception PC: %x \n", pc);

                reason = status & 0xF;

                switch(reason){
                    case 0x1: uart0_printf("Alignment Fault\n"); break;
                    case 0x4: uart0_printf("Translation Fault (Section)\n"); break;
                    case 0x5: uart0_printf("Translation Fault (Page)\n"); break;
                    case 0x8: uart0_printf("Permission Fault (Section)\n"); break;
                    case 0x9: uart0_printf("Permission Fault (Page)\n"); break;
                    default: uart0_printf("Unknown Fault\n"); break;
                }

                break;
            }
        case 3:  
            uart0_printf("Undefined Instruction Exception\n");
            break;
        case 4:  
            uart0_printf("Prefetch Abort Exception \n");
            break;
        default:
            uart0_printf("Unknown Exception\n");
            break;
    }

    /* can figure out some graceful exit method probably */
    uart0_printf("HALTING\n");

    while (1); 
}

/* change the timer interval to be 10 ms */
void new_timer_init(void) {
    WRITE32(DMTIMER0_BASE + DMTIMER0_TLDR, 0xFFFFFFD9);
    /* 0xFFFF8000 = 1s period, 0xFFFFFEB8 = 10 ms? */
    /* whatever the hell this number is is 1 ms ??? */

    /* Wait for the write to complete 
     * TWPS ensures register update is done
     */
    while (READ32(DMTIMER0_BASE + DMTIMER0_TWPS) & (0x4)) {}

    /* Set Timer Trigger Register (TTGR) to force a reload of TLDR 
     */
    WRITE32(DMTIMER0_BASE + DMTIMER0_TTGR, 0x1);

    /* Wait for write completion */
    while (READ32(DMTIMER0_BASE + DMTIMER0_TWPS) & (0x8)) {}
}

