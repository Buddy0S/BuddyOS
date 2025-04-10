/* CMPT432 - ImplementationTeam00 */

#ifndef MMC_H
#define MMC_H

#include <stdint.h>

typedef struct mmcdriver {
    volatile void (*init)(void);
    volatile void (*read_block)(uint32_t, volatile uint32_t*);
    volatile void (*write_block)(uint32_t, volatile uint32_t*);
} mmc_driver;

void initMMCdriver();
void MMCwriteblock(uint32_t block, volatile uint32_t* buf);
void MMCreadblock(uint32_t block, volatile uint32_t* buf);

#endif
