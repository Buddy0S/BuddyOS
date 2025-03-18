#include <stdint.h>
#include "uart.h"
#include "proc.h"

void dispatcher(void) {
    // run first process
    switch_context((unsigned int **)&current_process->kernel_sp, 
                   (unsigned int **)&current_process->stack_ptr);    
    while (1) {
        // svc handler gives code

        // call corresponding routine
    }
}
