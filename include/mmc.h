#ifndef MMC_H
#define MMC_H

#include <stdint.h>

typedef struct mmcdriver {
    void (*init)(void);
    void (*read_block)(uint32_t, uint32_t*);
    void (*write_block)(uint32_t, uint32_t*);
} mmc_driver;

void initMMCdriver();
void MMCwriteblock(uint32_t block, uint32_t* buf);
void MMCreadblock(uint32_t block, uint32_t* buf);

#endif
