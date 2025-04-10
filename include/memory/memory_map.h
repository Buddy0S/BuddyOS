/* CMPT432 - ImplementationTeam00 */

#ifndef MEMMAP_H
#define MEMMAP_H

#include <stdint.h>
extern uint32_t __sram_start__;
extern uint32_t __sram_size__;
extern uint32_t __stack_SVC_start__;
extern uint32_t __stack_SVC_size__;
extern uint32_t __stack_IRQ_start__;
extern uint32_t __stack_IRQ_size__;
extern uint32_t __stack_FRQ_start__;
extern uint32_t __stack_FRQ_size__;

#endif
