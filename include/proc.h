#ifndef PROC_H
#define PROC_H

#include <stdint.h>

/* Process states */
typedef enum {
    READY,
    RUNNING,
    /* We can add more states like BLOCKED, etc, later */
} ProcessState;


#endif // PROC_H

