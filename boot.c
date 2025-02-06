#include "boot.h"
#include "memory_map.h"
#include <stdint.h>

#define __GPIO1_start__ 0x4804C000
#define RESET_USERLED1 0x4804C190
#define SET_USERLED1 0x4804C194

int main(void) {

    buddy();
}

void initLED(void){

    /* set global register clock */
    *(volatile uint32_t*)((volatile char*)0x44E000AC) = 0x2;
    /* turn all gpio pins to output mode */
    *(volatile uint32_t*)((volatile char*)__GPIO1_start__ + 0x134) = 0x0;

}

void LEDon(void){

    *(volatile uint32_t*)((volatile char*)SET_USERLED1) = (uint32_t)(0xF << 21);
}

void LEDoff(void){

    *(volatile uint32_t*)((volatile char*)RESET_USERLED1) = (uint32_t)(0xF << 21);
}

void buddy(void) {

    initLED();
    
    LEDon();

    while(1);
}
