#include <stdint.h>
#include "uart.h"
#include "proc.h"

extern void schedule(void);

void dispatcher(void) {
    // run first process
    while (1) {
        uart0_printf("start of dispatcher\n");
        uart0_printf("last process sp %x\n", current_process->stack_ptr);
        uart0_printf("last process cpsr %x\n", current_process->cpsr);

        schedule();
        uart0_printf("next process sp %x\n", current_process->stack_ptr);
        uart0_printf("next process cpsr %x\n", current_process->cpsr);
        uart0_printf("jumping to %x\n", current_process->context.lr);


        if (current_process->started) {
            switch_to_svc(kernel_process, current_process);    
        } else {
            current_process->started = true;
            switch_to_start(kernel_process, current_process);    
        }

    }
}

void testprint(int32_t test) {
    uart0_printf("%x\n", test);
}
