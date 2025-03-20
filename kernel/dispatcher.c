#include <stdint.h>
#include "uart.h"
#include "proc.h"

extern void schedule(void);

void dispatcher(void) {
    // run first process
    while (1) {
        uart0_printf("start of dispatcher\n");

        register uint32_t r0 asm("r0");
        asm volatile("  \n\t    \
                mov r0, pc     \n\t    \
       " : : : "r0");

        uart0_printf("current pc ish: %x\n", r0);

        uart0_printf("last process sp %x\n", current_process->stack_ptr);

        schedule();
        uart0_printf("next process sp %x\n", current_process->stack_ptr);
        uart0_printf("jumping to %x\n", current_process->context.lr);


        if (current_process->started) {
            switch_to_svc(kernel_process, current_process);    
        } else {
            current_process->started = true;
            switch_to_start(kernel_process, current_process);    
        }

    }
}
