#include <stdint.h>
#include <stdbool.h>
#include "mm.h"

#define MAX_ORDER 15
#define MIN_BLOCK 128

/* addresses that we will be placing our data at */
const uint32_t order_arr_addr = KERNEL_RESERVED_START;
const uint32_t blocks_struct_addr = order_arr_addr + (sizeof(void *) * MAX_ORDER);

/* we gotta allocate memory for the array of lists via raw memory reference */ 
volatile struct mem_block **order_arr = (volatile struct mem_block **)order_arr_addr;
/* same here with the actual structs */
volatile struct mem_block *block_structs = (volatile struct mem_block *)blocks_struct_addr;
uint32_t block_struct_iterator = 0;

/*
 * data struct to store memory blocks as a linked list
 */
struct mem_block {
    bool used;
    uint32_t addr;
    struct free_list *next;
};

/*
 * returns a mem_block node
 */
struct mem_block *create_mem_block(void) {
    return &block_structs[block_struct_iterator++];
}

/*
 * we will have an array where each element corresponds to an order, each order
 * then has a linked list of blocks belonging to that order
 */
void init_alloc(void) {
    for (int i = 0; i < MAX_ORDER; i++) {
        order_arr[i] = create_mem_block();
        
        order_arr[i]->used = 0;
        order_arr[i]->addr = 0;
        order_arr[i]->next = NULL;
    }

    /* initialize the largest starting block */
    order_arr[0]->used = true;
    order_arr[0]->addr = KERNEL_START + KERNEL_SIZE; 
}

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


void *kmalloc(size_t size) {
    uint8_t order;
    order = find_order(size);
    
    /* create the blocks of the order we need */
    while (!order_arr[order]->addr) {
        
    }

    /* loop through the list searching for the block */
    while (order_arr[order]->next) {

    }
    
    return NULL
}

void kfree(void *) {

}
