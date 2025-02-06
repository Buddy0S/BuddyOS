#include "boot.h"
#include "memory_map.h"
#include <stdint.h>

#define __GPIO1_start__ 0x4804C000
#define RESET_USERLED1 0x4804C190
#define SET_USERLED1 0x4804C194

#define LED0 21
#define LED1 22
#define LED2 23
#define LED3 24

int main(void) {

    buddy();
}

void initLED(void){

    /* set global register clock */
    *(volatile uint32_t*)((volatile char*)0x44E000AC) = 0x2;
    /* turn all gpio pins to output mode */
    *(volatile uint32_t*)((volatile char*)__GPIO1_start__ + 0x134) = 0x0;

}

void LEDon(int led){

    uint32_t LEDstatus = *(volatile uint32_t*)((volatile char*)SET_USERLED1);

    *(volatile uint32_t*)((volatile char*)SET_USERLED1) = LEDstatus | (uint32_t)(0x1 << led);
}

void LEDoff(int led){

    *(volatile uint32_t*)((volatile char*)RESET_USERLED1) = (uint32_t)(0x1 << led);
}

void buddy(void) {

    initLED();
    
    LEDon(LED0);

    LEDon(LED2);

    while(1);
}
