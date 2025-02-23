#include "kernel.h"
#include "reg.h"
#include <stdint.h>

void kputc(char c){
    WRITE8(UART0_BASE, (uint8_t)c);
}
