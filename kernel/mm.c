#include <stdint.h>
#include "mm.h"

#define MAX_ORDER 15
#define MIN_BLOCK 128

/*
 * data struct to store memory blocks
 */
struct mem_list {
    uint32_t addr;
    struct free_list *next;
};

/*
 * helper function for buddy allocator:
 * finds the order of a value given an input
 */
uint8_t find_order(size_t n) {
    if (n == 0) return 0;       /* handles 0 case */
    n--;            /* handles case where n is a power of 2 */
    n |= n >> 1;    /* propagate the 1s  */ 
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;    /* convert to power of 2 */

    return (uint8_t)__builtin_ctz(n);    /* return the order */
}

void init_alloc(void) {
    struct mem_list blocks[MAX_ORDER];

}

void *kmalloc(size_t size) {
    uint8_t order;

    order = find_order(size);
    
    return NULL
}

void kfree(void *) {

}
