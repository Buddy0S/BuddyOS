MEMORY
{
  sram   (rwx)  : ORIGIN = 0x402F0400, LENGTH = 0x0000FC00
  stack_FRQ  (rwx)  : ORIGIN = 0x402F6000, LENGTH = 0x00002000
  stack_IRQ  (rwx)  : ORIGIN = 0x402F4000, LENGTH = 0x00002000
  stack_SVC  (rwx)  : ORIGIN = 0x402F2000, LENGTH = 0x00002000
  ocmc_ram (rwx)  : ORIGIN = 0x40300000, LENGTH = 0x00010000
}


__sram_start__ = ORIGIN(sram);
__sram_size__ = LENGTH(sram);
__stack_SVC_start__ = ORIGIN(ocmc_ram);
__stack_SVC_size__ = LENGTH(stack_SVC);
__stack_IRQ_start__ = ORIGIN(stack_SVC);
__stack_IRQ_size__ = LENGTH(stack_IRQ);
__stack_FRQ_start__ = ORIGIN(stack_IRQ);
__stack_FRQ_size__ = LENGTH(stack_FRQ);
ENTRY(init);

SECTIONS 
{
    . = 0x0;
    .data :
    {
        LONG(SIZEOF(.data) - 0x8);
        LONG(__sram_start__);
        . = 0x8;
        build/init.o(.text);
        *(.text);
    }
}
