/* CMPT432 - ImplementationTeam00 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

/* Interrupt Controler base address */
#define INTERRUPTC_BASE 0x48200000

/* Interrupt Controler offsets */
#define INTC_SYSCONFIG 0x10
#define INTC_SYSSTATUS 0x14

#define INTC_IRQ 0x40

#define INTC_CONTROL 0x48

#define INTC_IDLE 0x50

#define INTC_MIR0 0x84
#define INTC_MIR_CLEAR0 0x88
#define INTC_MIR_SET0 0x8C
#define INTC_ISR_CLEAR0 0x90

#define INTC_MIR1 0xA4
#define INTC_MIR_CLEAR1 0xA8
#define INTC_MIR_SET1 0xAC
#define INTC_ISR_CLEAR1 0xB4

#define INTC_MIR2 0xC4
#define INTC_MIR_CLEAR2 0xC8
#define INTC_MIR_SET2 0xCC
#define INTC_ISR_CLEAR2 0xD4

#define INTC_MIR3 0xE4
#define INTC_MIR_CLEAR3 0xE8
#define INTC_MIR_SET3 0xEC
#define INTC_ISR_CLEAR3 0xF4

#define INTC_ILRm_BASE 0x100

/*TI manual 6.2.1*/

void init_interrupts(void);

void enable_interrupts(void);

void disable_interrupts(void);

#endif
