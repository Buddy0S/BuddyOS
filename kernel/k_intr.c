#include "proc.h"
#include "interrupts.h"
#include "led.h"
#include "timer.h"
#include "syscall.h"
#include "uart.h"
#include <stdint.h>

uint32_t test_syscall(int a, int b) {
    return a + b;
}

/* called by supervisor vector in idt when svc interrupt is raised.
 *
 * supervisor vector in vector_table.S passes syscall number and function
 * arguments to this handler.
 *
 * marked as void return but may place a return value into r0 which will
 * then be returned to the caller by the supervisor vector. */
void svc_handler(uint32_t svc_num, uint32_t args[]) {
    // arguments will be put into the args array, but technically its just
    // a pointer to the bottom of the process stack that performed the syscall
    uart0_printf("you just called a syscall my good buddy\n");
    register uint32_t sp asm("sp");
    uart0_printf("current sp: %x\n", sp);
    uart0_printf("syscall num: %d\n", svc_num);
    uart0_printf("r0: %d\n", args[0]);
    uart0_printf("r1: %d\n", args[1]);
    uart0_printf("r2: %d\n", args[2]);
    uart0_printf("r3: %d\n", args[3]);
    // to give a return value for the system call, put it into args[0]
    switch (svc_num) {
        case SYSCALL_TEST_2_ARGS_NR: // can definitely replace this number later
            // takes two arguments from r0 and r1 and returns their sum
            args[0] = test_syscall(args[0], args[1]);
            break;
        case SYSCALL_YIELD_NR:
            yield();
            break;
        default:
            uart0_printf("invalid or unimplemented syscall\n");
            break;

    }
    return;
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


    uart0_printf("IRQ number %d\n", (int) irqnum);

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

void kexception_handler(uint32_t exception) {
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

                reason = status & 0xF;

		switch(reason){
		    case 0x0: uart0_printf("Alignment Fault\n"); break;
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

    uart0_printf("HALTING\n");

    while (1); 
}


