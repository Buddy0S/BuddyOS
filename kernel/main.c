#include <stdint.h>
#include "reg.h"
#include "kernel.h"
#include "uart.h"

int main(){

    kputs("I hope this works");

    uart0_printf("testing noahs print function\n");

    while(1){}	

    return 0;
}
