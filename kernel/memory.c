#include <stdint.h>
#include <stdbool.h>
#include "memory.h"
#include "uart.h"

#define MAX_ORDER 6
#define MIN_BLOCK 64
#define MAX_BLOCK 2048
#define BLOCK_NUM ((KERNEL_DYNAMIC / 2) / MAX_BLOCK)
#define NULL ((void*)0)


/* addresses that we will be placing our data at */
const uint32_t order_arr_addr = KERNEL_RESERVED_START;
const uint32_t list_struct_addr = order_arr_addr + ((sizeof(void *) * MAX_ORDER));
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
    uint32_t num_free;
};

/*
 * returns a mem_block node.
 *
 * the node is stored in the starting bytes of the free node it is referencing.
 * this way, when giving out this block, we can clear the node and use have
 * no memory overhead.
 */
struct mem_block *create_mem_block(int order) {
    if (blocks_struct_addr > (KERNEL_DYNAMIC_START + KERNEL_DYNAMIC)) {
        uart0_puts("no more mem\n");
        return NULL;
    }
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
    order_arr[order]->num_free = BLOCK_NUM;
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

uint8_t ctz(uint8_t x) {
    uint8_t count = 0;
    while ((x & 1) == 0 && x != 0) {
        x >>= 1;
        count++;
    }
    return count;
}

/* find order given size */
uint8_t find_order(uint32_t n) {
    if (n <= MIN_BLOCK) {
        return 0;
    }
    n--;            /* handles case where n is a power of 2 */
    n |= n >> 1;    /* propagate the 1s  */ 
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;    /* convert to power of 2 */

    return ctz((n / MIN_BLOCK));    /* return the order */
}

/* find size given addr */
uint32_t find_size(uint32_t addr) {
    addr = addr - KERNEL_DYNAMIC_START;
    uint32_t size = MIN_BLOCK;
    uint32_t bound = (size * BLOCK_NUM);
    while (size <= MAX_BLOCK) {
        if (addr < bound) {
            return size;
        } else {
            size = size << 1;
            bound += (size * BLOCK_NUM);
        }
    }
    return -1;
}

/* simple memset */
void memset32(uint32_t addr, uint32_t value, uint32_t size) {
    uint32_t *ptr = (uint32_t *)addr;
    for (uint32_t i = 0; i < size / sizeof(uint32_t); i++) {
        ptr[i] = value;
    }
}

void *kmalloc(uint32_t size) {
    uint8_t order;
    uint32_t addr;
    int block_size;
    struct mem_block *alloc_block;

    if (size > MAX_BLOCK || size <= 0) {
        uart0_puts("invalid size\n");
        return NULL;
    }
    
    order = find_order(size);
    block_size = (MIN_BLOCK << order);

    if (order_arr[order]->num_free <= 0) {
        uart0_puts("no more free blocks of this order\n");
        return NULL;
    }

    alloc_block = order_arr[order]->head;
    if (alloc_block == NULL) {
        return NULL;
    }
    order_arr[order]->head = alloc_block->next;

    /* empty */
    if (order_arr[order]->head == NULL) {
        order_arr[order]->tail = NULL;
    }

    addr = alloc_block->addr;

    memset32(addr, 0, block_size);
    uart0_puts("kmalloc reached return\n");
    order_arr[order]->num_free--;
    return (void *)addr;
}

int kfree(void *ptr) {
    uint8_t order;

    /* free block struct ptr and assigning */
    struct mem_block *free_block = (struct mem_block *)ptr;
    
    free_block->addr = (uint32_t)ptr;
    free_block->size = find_size(free_block->addr);
    free_block->next = NULL;
    order = find_order(free_block->size);

    if (!order_arr[order]) {
        return -1;
    }
    if (!order_arr[order]->head) {
        order_arr[order]->head = free_block;
        order_arr[order]->tail = free_block;
    } else {
        order_arr[order]->tail->next = free_block;
        order_arr[order]->tail = free_block;
    }
    uart0_puts("kfree reached return\n");
    order_arr[order]->num_free++;
    return 0;

}
