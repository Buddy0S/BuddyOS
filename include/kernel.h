#ifndef KERNEL_H
#define KERNEL_H

#define UART0_BASE 0x44E09000

void kputc(char c);
void kputs(const char *s);

#endif
