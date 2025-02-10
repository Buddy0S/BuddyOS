#ifndef MM_H
#define MM_H

#define KERNEL_START  0x80000000
#define KERNEL_SIZE   0x00400000  /* 4MB for the time being */
#define KERNEL_DYNAMIC     0x00400000  /* 4MB of dynamic mem */
#define ALLOC_START   (KERNEL_START + KERNEL_SIZE + KERNEL_DYNAMIC)
#define ALLOC_END     0x8FFFFFFF


#endif
