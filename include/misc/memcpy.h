#ifndef MEMCPY_H
#define MEMCPY_H

#include <stdint.h>

void bmemcpy(void* dest, const void* const src, unsigned long len);
void bmemset(void *dest, uint32_t T, unsigned long len);


#endif
