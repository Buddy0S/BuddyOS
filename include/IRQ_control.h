#ifndef IRQ_CONTROL_H
#define IRQ_CONTROL_H

static inline void enable_IRQ() {
    asm("cpsie i");
}

static inline void disable_IRQ() {
    asm("cpsid i");
}

#endif // !IRQ_CONTROL_H
