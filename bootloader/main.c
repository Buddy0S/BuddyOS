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


                asm volatile ("mrc p15, 0, %0, c6, c0, 0" : "=r" (addr));

                asm volatile ("mrc p15, 0, %0, c5, c0, 0" : "=r" (status));

                uart0_printf("Addr: %x \n", addr);
                uart0_printf("Status: %x \n", status);

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
    
    uint32_t __attribute__((aligned(4))) buffer[256];

    uart0_printf("Attempting to init fat12......\n");
    fat12_init(0, buffer);

	fat12_create_dir_entry("test.txt", 0x20, buffer);

	fat12_write_file("test.txt","The quick brown fox jumps over the lazy dog near the riverbank every morning without fail. This simple sentence has been used for years to test typewriters, keyboards, and fonts because it contains every letter of the alphabet at least once, making it a perfect tool for such purposes. Beyond that, it tells a brief story of a fox and a dog, two animals living their daily lives in a peaceful coexistence by the water's edge. Imagine the scene: the sun is just rising, casting a golden glow over the rippling river, while the fox, full of energy, leaps gracefully over the resting dog, who barely stirs from his slumber. This little tale could go on, exploring their adventures, their friendship, or even the other creatures they meet, like the birds chirping overhead or the fish swimming below. It’s a small snapshot of nature, painted with words, that stretches just far enough to fill the space we need.", 909, buffer);

    fat12_read_file("test.txt", buffer, buffer);

	uart0_printf("Read from test.txt - %s", buffer);

	fat12_read_file("KERNEL.BIN", (uint32_t *)0x80000000, buffer);

    /*jump to kernel*/
    uint32_t* kernel = (uint32_t*)0x80000000;
    KernelStart kernelStart = (KernelStart)kernel;

    uart0_printf("Jumping to kernel \n");

    kernelStart();

    uart0_printf("Kernel Exited\n"); 

    while (1);
}


