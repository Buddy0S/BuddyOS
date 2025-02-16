#include <stdint.h>
#include <stdbool.h>
#include "mm.h"

#define MAX_ORDER 5
#define MIN_BLOCK 64
#define MAX_BLOCK 1024
#define BLOCK_NUM ((KERNEL_RESERVED / 2) / MAX_BLOCK)
#define NULL ((void*)0)


/* addresses that we will be placing our data at */
const uint32_t order_arr_addr = KERNEL_RESERVED_START;
const uint32_t list_struct_addr = order_arr_addr + ((sizeof(void *) * MAX_ORDER) + 1);
static uint32_t blocks_struct_addr = KERNEL_DYNAMIC_START;

/* we gotta allocate memory for the array of lists via raw memory reference */ 
volatile struct mem_list **order_arr = (volatile struct mem_list **)order_arr_addr;
volatile struct mem_list *list_structs = (volatile struct mem_list *)list_struct_addr;

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
};

/*
 * returns a mem_block node.
 *
 * the node is stored in the starting bytes of the free node it is referencing.
 * this way, when giving out this block, we can clear the node and use have
 * no memory overhead.
 */
struct mem_block *create_mem_block(int order) {
    volatile struct mem_block *block_structs = (volatile struct mem_block *)blocks_struct_addr;
    blocks_struct_addr += (MIN_BLOCK << order);
    return (struct mem_block *)block_structs;
}

/*
 * helper function to initialize each linked list
 */
int init_order_arr(int order, int size, uint32_t addr) {
    struct mem_block *new_tail;
    uint32_t i, end;
    end = addr + (size * BLOCK_NUM);

    /* initialize the list head and tail */
    order_arr[order] = &list_structs[order];
    
    /* set the first list head and tail */
    order_arr[order]->head = create_mem_block(order);
    if (order_arr[order]->head == NULL) {
        return -1;
    }
    order_arr[order]->tail = order_arr[order]->head;
    order_arr[order]->tail->size = size;
    order_arr[order]->tail->addr = addr;
    order_arr[order]->tail->next = NULL;
    addr += size;

    /* set the rest of the free blocks */
    i = addr;
    while (i < end) {
        new_tail = create_mem_block(order);
        if (new_tail == NULL) {
            return -1;
        }
        order_arr[order]->tail->next = new_tail;
        new_tail->size = size;
        new_tail->addr = i;
        new_tail->next = NULL;
        order_arr[order]->tail = new_tail;
        i += size;
    }
    return 0;
}

/*
 * we will have an array where each element corresponds to an order, each order
 * then has a linked list of blocks belonging to that order
 */
int init_alloc(void) {
    uint32_t order, addr, block_size;


    /* initialize each free stack */
    addr = KERNEL_DYNAMIC_START;
    for (order = 0; order < MAX_ORDER; ++order) {
        block_size = (MIN_BLOCK << order);
        if (init_order_arr(order, block_size, addr) != 0) {
            return -1;
        }
        addr += block_size * BLOCK_NUM;
    }
    return 0;
}

/* find order given size */
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

/* find size given addr */
uint32_t find_size(uint32_t addr) {
    addr = addr - KERNEL_DYNAMIC_START;
    int size = MAX_BLOCK;
    int bound = (size * NUM_BLOCK);
    while (size >= MIN_BLOCK) {
        if (addr < bound) {
            return size;
        } else {
            size >> 1;
            bound += (size * NUM_BLOCK);
        }
    }
    return -1;
}

/* simple memset */
void memset32(uint32_t addr, uint32_t value, size_t size) {
    uint32_t *ptr = (uint32_t *)addr;
    for (size_t i = 0; i < size / sizeof(uint32_t); i++) {
        ptr[i] = value;
    }
}

void *kmalloc(size_t size) {
    uint8_t order;
    uint32_t addr;
    int block_size;
    void *alloc_block;
    order = find_order(size);
    block_size = (MIN_BLOCK << order);
    
    alloc_block = order_arr[order]->head;
    if (alloc_block == NULL) {
        return NULL;
    }
    order_arr[order]->head = alloc_block->next;

    addr = alloc_block->addr;

    memset32(addr, 0, block_size);

    return (void *)addr;
}

void kfree(void *ptr) {
    volatile struct mem_block *free_block = (volatile struct mem_block *)ptr;
    free_block->addr = (uint32_t)ptr;
    free_block->size = 
}
