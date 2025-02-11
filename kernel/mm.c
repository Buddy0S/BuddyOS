#include <stdint.h>
#include <stdbool.h>
#include "mm.h"

#define MAX_ORDER 5
#define MIN_BLOCK 64
#define MAX_BLOCK 1024
#define BLOCK_NUM ((KERNEL_RESERVED / 2) / MAX_BLOCK)

/* addresses that we will be placing our data at */
const uint32_t order_arr_addr = KERNEL_RESERVED_START;
const uint32_t list_struct_addr = order_arr_addr + ((sizeof(void *) * MAX_ORDER) + 1);
const uint32_t blocks_struct_addr = list_struct_addr + ((sizeof(mem_list) * MAX_ORDER) + 1);

/* we gotta allocate memory for the array of lists via raw memory reference */ 
volatile struct mem_list **order_arr = (volatile struct mem_list **)order_arr_addr;
volatile struct mem_list *list_structs = (volatile struct mem_list **)list_struct_addr;
volatile struct mem_block *block_structs = (volatile struct mem_block *)blocks_struct_addr;
uint32_t block_struct_iterator = 0;

/*
 * data structs to store memory blocks as a linked list
 */
struct mem_block {
    uint32_t addr;
    uint32_t size;
    struct mem_block *next;
};

struct mem_list {
    struct mem_block *head;
    struct mem_block *tail;
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
};

/*
 * returns a mem_block node
 */
struct mem_block *create_mem_block(void) {
    return &block_structs[block_struct_iterator++];
}

/*
 * helper function to initialize each linked list
 */
void init_order_arr(int order, int size, uint32_t addr) {
    struct mem_block *new_tail;
    uint32_t i, end;
    end = addr + (size * BLOCK_NUM);

    order_arr[order]->head = create_mem_block();
    order_arr[order]->tail = order_arr[order]->head;
    order_arr[order]->tail->size = size;
    order_arr[order]->tail->addr = addr;
    order_arr[order]->tail->next = NULL;
    addr += size;

    i = addr;
    while (i < end) {
        new_tail = order_arr[order]->tail->next;
        new_tail = create_mem_block();
        new_tail->size = size;
        new_tail->addr = i;
        new_tail->next = NULL;
        order_arr[order]->tail = new_tail;
        i += size;
    }   
}

/*
 * we will have an array where each element corresponds to an order, each order
 * then has a linked list of blocks belonging to that order
 */
void init_alloc(void) {
    int mem_end = KERNEL_RESERVED_START + KERNEL_RESERVED;
    uint32_t order0 = KERNEL_RESERVED + 131072;
    uint32_t order1 = order0 + 262144;
    uint32_t order2 = order1 + 524288;
    uint32_t order3 = order2 + 1048576;
    uint32_t order4 = order3 + 2097152;

    /* initialize each free stack */
    for (int i = 0; i < MAX_ORDER; ++i) {
        order_arr[i] = &list_structs[i];
        order_arr[i]->head = NULL;
        order_arr[i]->tail = NULL;
    }

}




void *kmalloc(size_t size) {
}

void kfree(void *) {

}
