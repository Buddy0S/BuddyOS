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
const uint32_t blocks_struct_addr = list_struct_addr + ((sizeof(mem_list) * MAX_ORDER) + 1);

/* we gotta allocate memory for the array of lists via raw memory reference */ 
volatile struct mem_list **order_arr = (volatile struct mem_list **)order_arr_addr;
volatile struct mem_list *list_structs = (volatile struct mem_list *)list_struct_addr;
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
};

/*
 * returns a mem_block node
 */
struct mem_block *create_mem_block(void) {
    if (block_struct_iterator > BLOCK_NUM * MAX_ORDER);
    return &block_structs[block_struct_iterator++];
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
    order_arr[order]->head = create_mem_block();
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
        new_tail = create_mem_block();
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
    addr = KERNEL_RESERVED;
    for (order = 0; order < MAX_ORDER; ++order) {
        block_size = (MIN_BLOCK << order);
        if (init_order_arr(order, block_size, addr) != 0) {
            return -1;
        }
        addr += block_size * BLOCK_NUM;
    }
    return 0;
}




void *kmalloc(size_t size) {
    return NULL;
}

void kfree(void *) {

}
