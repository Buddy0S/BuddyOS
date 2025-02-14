#include <stdint.h>
#include "interrupts.h"
#include "memcpy.h"
#include "timer.h"
#include "led.h"
#include "uart.h"
#include "clock.h"
#include "ddr.h"
#include "mmc.h"

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

int main(void) { 
	
    initClocks();

    init_interrupts();

    init_ddr();

    initLED();

    uart0_init();

    uart0_printf("INITING\n");
    initTimer();

    enable_interrupts();

    initMMCdriver();

    uint32_t bufW[128];
    uint32_t bufR[128];

    //mem set is broken will cause program to stall, TODO
    //bmemset(bufW,7,128);

    bufW[1] = 7;

    uart0_printf("writing to block 1 \n");

    MMCwriteblock(1,bufW);

    uart0_printf("reading form block 1 \n");

    MMCreadblock(1,bufR);

    uart0_printf("checking %d\n",bufR[1]);

    const char* test = "testing!";
    const char* initializeMsg = "BuddyOS...initialized...";

    bmemcpy((void*)0x80000000, initializeMsg, 25);

    uart0_puts((char*)0x80000000);

    uart0_test(); 
    while (1);
}


