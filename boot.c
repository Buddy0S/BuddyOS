#include "boot.h"
#include "memory_map.h"
#include <stdint.h>

#define __GPIO1_start__ 0x4804C000
#define RESET_USERLED1 0x4804C190
#define SET_USERLED1 0x4804C194

int main(void) {
    buddy();

    set_registers((uint32_t)buddy, __stack_start__);
    while(1);
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
            mov r13, r1\n\
            bx r0      \n\
    ");
}

void buddy(void) {
    /* set global register clock */
    *(volatile uint32_t*)((volatile char*)0x44E000AC) = 0x2;
    /* turn all gpio pins to output mode */
    *(volatile uint32_t*)((volatile char*)__GPIO1_start__ + 0x134) = 0x0;
    /* enable all pins */
    *(volatile uint32_t*)((volatile char*)SET_USERLED1) = (uint32_t)0xFFFFFFFF;
    *(volatile uint32_t*)((volatile char*)SET_USERLED1) = 1 << 23;
    *(volatile uint32_t*)((volatile char*)SET_USERLED1) = 1 << 22;
    *(volatile uint32_t*)((volatile char*)SET_USERLED1) = 1 << 21;

    while(1);
}
