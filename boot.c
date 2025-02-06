#include "boot.h"
#include "led.h"
#include "memory_map.h"
#include <stdint.h>

#define LED0 21
#define LED1 22
#define LED2 23
#define LED3 24

int main(void) {
    /* literally no point in doing this rn but it works */
    set_registers((uint32_t)buddy, (uint32_t)&__stack_start__);
}
/* 
 * argument 1 is in r0 takes function
 * argument 2 is in r1 takes address for stack
 *
 * r13 is register for sp
 *
 * moves argument for r1 into sp register (r13)
 *
 * branches to function in r0
 *
 * */
__attribute__((naked, noreturn)) static void set_registers(uint32_t pc, uint32_t sp) {
    __asm("            \n\
            mov sp, r1 \n\
            bx r0     \n\
    ");
}
void buddy(void) {

    volatile int i;
    volatile int T = 500000;

    initLED();
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

    while(1);
}
