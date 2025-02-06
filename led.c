#include "led.h"
#include <stdint.h>

inline void initLED(void){
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


