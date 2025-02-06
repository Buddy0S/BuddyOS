#include "boot.h"
#include "led.h"
#include "memory_map.h"
#include <stdint.h>

#define __GPIO1_start__ 0x4804C000
#define SET_LED_OFFSET 0x194
#define RESET_LED_OFFSET 0x190
#define IO_MODE_OFFSET 0x134

#define GLBL_GPIO_CLOCK 0x44E00000

#define SET_USERLED1 __GPIO1_start__ + SET_LED_OFFSET
#define RESET_USERLED1 __GPIO1_start__ + RESET_LED_OFFSET

#define LED0 21
#define LED1 22
#define LED2 23
#define LED3 24

int main(void) {
    buddy();
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
__attribute__((naked)) static void set_registers(uint32_t pc, uint32_t sp) {
    __asm("            \n\
            mov sp, r1 \n\
            bx r0     \n\
    ");
}

static void initLED(void){

    /* set global register clock */
    /* lowkey have no idea what 0xAC is the offset for */
    *(volatile uint32_t*)((volatile char*)GLBL_GPIO_CLOCK + 0xAC) = 0x2;
    /* turn all gpio pins to output mode */
    *(volatile uint32_t*)((volatile char*)__GPIO1_start__ + IO_MODE_OFFSET) = 0x0;

}

inline void LEDon(int led){
    *(volatile uint32_t*)((volatile char*)SET_USERLED1) = (uint32_t)(0x1 << led);
}

inline void LEDoff(int led){
    *(volatile uint32_t*)((volatile char*)RESET_USERLED1) = (uint32_t)(0x1 << led);
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
