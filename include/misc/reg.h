/* CMPT432 - ImplementationTeam00 */

#ifndef REG_H

#include <stdint.h>

void WRITE32(uint32_t addr, uint32_t val);
void WRITE16(uint32_t addr, uint16_t val);
void WRITE8(uint32_t addr, uint8_t val);

uint32_t READ32(uint32_t addr);
uint16_t READ16(uint32_t addr);
uint8_t READ8(uint32_t addr);

#endif
