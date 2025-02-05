#include "boot.h"
#include "memory_map.h"


#define __GPIO1_start__ 0x4804C000
#define RESET_USERLED1 __GPIO1_start__ + 0x190
#define SET_USERLED1 __GPIO1_start__ + 0x194

int main(void) {
    set_registers((uint32_t)buddy, __stack_start__);
    while(1);
}

__attribute__((naked)) static void set_registers(uint32_t pc, uint32_t sp) {
    __asm("            \n\
            mov r13, r1\n\
            bx r0      \n\
    ");
}

void buddy(void) {
    while(1){
        *((volatile char*)SET_USERLED1) = 0xFFFFFFFF;
        *((volatile char*)RESET_USERLED1) = 0xFFFFFFFF;
    }
}
