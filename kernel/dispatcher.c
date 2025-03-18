#include <stdint.h>
#include "uart.h"
#include "proc.h"

extern void schedule(void);

void dispatcher(void) {
    // run first process
    while (1) {
        uart0_printf("start of dispatcher\n");
        uart0_printf("kernel sp %x\n", kernel_process.stack_ptr);
        schedule();
        uart0_printf("next process sp %x\n", current_process->stack_ptr);
        uart0_printf("jumping to %x\n", current_process->context.lr);



        switch_context(&kernel_process, current_process);    
    }
}

void testprint(int32_t test) {
    uart0_printf("%x\n", test);
}
