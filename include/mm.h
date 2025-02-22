#ifndef MM_H
#define MM_H

#define KERNEL_START        0x80000000
#define KERNEL_SIZE         0x00400000  /* 4MB */
#define KERNEL_RESERVED     0x00400000  /* 4MB */
#define KERNEL_DYNAMIC      0x00400000  /* 4MB */

#define KERNEL_TOTAL        (KERNEL_START + KERNEL_SIZE + KERNEL_RESERVED + KERNEL_DYNAMIC)

#define KERNEL_RESERVED_START (KERNEL_START + KERNEL_SIZE)
#define KERNEL_DYNAMIC_START (KERNEL_RESERVED_START + KERNEL_RESERVED)

#define ALLOC_START         (KERNEL_START + KERNEL_TOTAL)
#define ALLOC_END           0x8FFFFFFF

int init_alloc(void);
void *kmalloc(uint32_t size);
int kfree(void *ptr);

#endif
