#pragma once
#include <stdint.h>
__attribute__((naked)) static void set_registers(uint32_t pc, uint32_t sp);
void buddy(void);
