#include <stdint.h>
#include "interrupts.h"
#include "timer.h"
#include "led.h"
#include "uart.h"
#include "clock.h"

void buddy(void) {

    volatile int i;
    volatile int T = 500000;

    while(1){

        LEDon(LED0);
        //uart0_putch('T');

       /* for (i = 0; i < T; i++);

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

	*/
    }

}

int main(void) { 

    initClocks();

    init_interrupts();

    initLED();

    uart0_init();

    initTimer();

    enable_interrupts();

    //buddy();

    const char* initializeMsg = "BuddyOS...initialized...";

    uart0_puts(initializeMsg);

    while (1);
}


