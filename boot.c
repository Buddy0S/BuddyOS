#include "boot.h"
#include "led.h"
#include "memory_map.h"
#include <stdint.h>
#include "interrupts.h"

int main(void) {

    init_interrupts();

    initLED();

    enable_interrupts();

    buddy();
    
}

void buddy(void) {

    volatile int i;
    volatile int T = 500000;

    while(1){
    	
	LEDon(LED0);

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

}
