#include <stdint.h>
#include "interrupts.h"
#include "memcpy.h"
#include "timer.h"
#include "led.h"
#include "uart.h"
#include "clock.h"
#include "ddr.h"
#include "mmc.h"
#include "fat12.h"

extern mmc_driver mmc;

void exception_handler(uint32_t exception) {
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


		//asm volatile ("mrc p15, 0, %0, c6, c0, 0" : "=r" (addr));

		asm volatile ("mrc p15, 0, %0, c5, c0, 0" : "=r" (status));

		//uart0_printf("Addr: %x \n", addr);
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

void buddy(void) {

    volatile int i;
    volatile int T = 500000;

    for (i = 0; i < T; i++);

    LEDon(LED1);

    for (i = 0; i < T; i++);

    LEDon(LED2);

    for (i = 0; i < T; i++);

    LEDon(LED3);

    for (i = 0; i < T; i++);

    LEDoff(LED0);

    for (i = 0; i < T; i++);

    LEDoff(LED1);

    for (i = 0; i < T; i++);

    LEDoff(LED2);

    for (i = 0; i < T; i++);

    LEDoff(LED3);

    for (i = 0; i < T; i++);

}

typedef void (*KernelStart)();

int main(void) { 
	
    initClocks();

    init_interrupts();

    init_ddr();

    initLED();

    uart0_init();

    uart0_printf("INITING\n");
    initTimer();

    enable_interrupts();

    mmc.init();
    
    //initMMCdriver();

    //volatile uint32_t __attribute__((aligned(4))) bufW[128];
    //volatile uint32_t __attribute__((aligned(4))) bufR[128];

    //mem set is broken will cause program to stall, TODO
    //bmemset(bufW,7,128);

    //const char* initializeMsg = "BuddyOS...initialized...";

    //bmemcpy((void*)0x80000000, initializeMsg, 25);

    //uart0_puts((char*)0x80000000);

    uart0_test();

    volatile uint32_t __attribute__((aligned(4))) buffer[128];

    uart0_printf("Attempting to init fat12......\n");
    fat12_init(0, buffer);

    volatile uint16_t __attribute__((aligned(4))) startCluster = 0;
    volatile uint32_t __attribute__((aligned(4))) size = 0;

    //volatile uint32_t __attribute__((aligned(4))) buff[128];

    ////////////////////////////////////////////////
    // THIS DOES NOT WORK
    /*
    volatile uint32_t __attribute__((aligned(4))) retVal = 0;
    retVal = fat12_find("HELLO.TXT", buff, &startCluster, &size);
    
    if (retVal) {
        uart0_printf("Found! Start = %d, size = %d\n", startCluster, size);
    }
    else {
        uart0_printf("Not Found :(\n");
    }
    */
    ///////////////////////////////////////////////

    uart0_printf("Attempting find..\n");
    fat12_find("HELLO.TXT", buffer, &startCluster, &size);
    uart0_printf("Start Cluster: %d, File Size: %d\n", startCluster, size);

    uart0_printf("About to start read file test\n");
    volatile uint32_t recvBuff[128];

	fat12_read_file("HELLO.TXT", recvBuff);
    uart0_printf("%s\n", recvBuff);

    uint32_t bytesRead = fat12_read_file("KERNEL.BIN", (volatile uint32_t *)0x80000000);

    /*jump to kernel*/
    uint32_t* kernel = (uint32_t*)0x80000000;
    KernelStart kernelStart = (KernelStart)kernel;

    uart0_printf("Jumping to kernel \n");

    kernelStart();

    uart0_printf("Kernel Exited\n"); 

    while (1);
}


