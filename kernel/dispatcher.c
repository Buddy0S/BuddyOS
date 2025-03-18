#include <stdint.h>
#include "uart.h"
#include "proc.h"

void dispatcher(void) {
    // run first process
    while (1) {
        switch_context(&kernel_process, current_process);    
    }
}
