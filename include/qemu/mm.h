#ifndef MM_H
#define MM_H

#define KERNEL_START 0x80000000
#define KERNEL_SIZE  0x00400000  /* 4MB for the time being */
#define RESERVED_SIZE 0x00400000 /* reserve for kernel data structures */
#define ALLOC_START  (KERNEL_START + KERNEL_SIZE + RESERVED_SIZE)

#endif
