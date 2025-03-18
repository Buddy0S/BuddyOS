#include <stdint.h>
#include "uart.h"
#include "proc.h"

void dispatcher(void) {
    // run first process
    while (1) {
        uart0_printf("start of dispatcher\n");
        uart0_printf("kernel sp %x\n", kernel_process.stack_ptr);
        uart0_printf("next process sp %x\n", current_process->stack_ptr);

        switch_context(&kernel_process, current_process);    
    }
}
