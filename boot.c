#include "boot.h"
#include "memory_map.h"
#include <stdint.h>

#define __GPIO1_start__ 0x4804C000
#define RESET_USERLED1 0x4804C190
#define SET_USERLED1 0x4804C194

int main(void) {
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

static inline void set_bit_sram(uint32_t addr, unsigned int bit) {
        *(uint32_t*)((volatile char*)addr) |= 1 << bit;
}

void buddy(void) {
        *(volatile uint32_t*)((volatile char*)__GPIO1_start__ + 0x134) = 0x0;
    while(1){
        *(volatile uint32_t*)((volatile char*)RESET_USERLED1) = 0xFFFFFFFF;
    }
}
