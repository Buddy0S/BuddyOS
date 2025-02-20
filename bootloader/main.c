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

    mmc.init();
    
    //initMMCdriver();

    volatile uint32_t __attribute__((aligned(4))) bufW[128];
    volatile uint32_t __attribute__((aligned(4))) bufR[128];

    //mem set is broken will cause program to stall, TODO
    //bmemset(bufW,7,128);

    bufW[1] = 7;

    uart0_printf("writing to block 1 \n");

    mmc.write_block(1,bufW);

    uart0_printf("reading form block 1 \n");

    mmc.read_block(1,bufR);

    uart0_printf("checking %d\n",bufR[1]);

    const char* test = "testing!";
    const char* initializeMsg = "BuddyOS...initialized...";

    bmemcpy((void*)0x80000000, initializeMsg, 25);

    uart0_puts((char*)0x80000000);

    uart0_test();

    volatile uint32_t __attribute__((aligned(4))) buffer[128];

    uart0_printf("Attempting to init fat12......\n");
    fat12_init(0, buffer);

    //volatile uint16_t __attribute__((aligned(4))) startCluster[1];
    //volatile uint32_t __attribute__((aligned(4))) size[1];
    
    volatile uint16_t __attribute__((aligned(4))) startCluster;
    volatile uint32_t __attribute__((aligned(4))) size;

    //uint16_t startCluster;
    //uint32_t size;
    
    volatile uint32_t __attribute__((aligned(4))) buff[128];

    ////////////////////////////////////////////////
    // THIS DOES NOT WORK
    /*
    volatile int retVal;
    retVal = fat12_find("HELLO.TXT", buff, &startCluster, &size);
    */
    ///////////////////////////////////////////////

    /* THIS WORKS......NO CLUE WHY */
    fat12_find("HELLO.TXT", buff, &startCluster, &size);
    
    /*
    if (retVal) {
        uart0_printf("Found! Start = %d, size = %d\n", startCluster, size);
    }
    else {
        uart0_printf("Not Found :(\n");
    }
    */
    

    while (1);
}


