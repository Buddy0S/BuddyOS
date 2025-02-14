#include "memcpy.h"
#include "reg.h"
#include <stdint.h>

void bmemcpy(void *dest, const void *const src, unsigned long len) {

    for (int i = 0; i < len; ++i) {
        WRITE8((uint32_t)dest + i, *((uint8_t*)src + i));
    }

}

void bmemset(void *dest, uint32_t T, unsigned long len) {

    for (int i = 0; i < len; ++i) {
        WRITE32((uint32_t)dest + i, T);
    }

}
