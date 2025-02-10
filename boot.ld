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

__vector_load__ = __sram_start__ + SIZEOF(.header);
/* enter at reset vector */
ENTRY(vector_table);

SECTIONS 
{
    .header :
    {
        LONG(SIZEOF(.text) + SIZEOF(.bss) + SIZEOF(.data) + SIZEOF(.rodata));
        LONG(__sram_start__ + 8);
    } > sram
    . = __sram_start__ + 8;
    . = ALIGN(32);
    __vector_load__ = .;
    .text :
    {    
	build/vector_table.o(.text);    
        . = ALIGN(4);
        build/init.o(.text);
        build/main.o(.text.main);
        *(.text);
    } > sram
    .rodata :
    {
	. = ALIGN(4);
        *(.rodata);
    } > sram
    .bss :
    {
	. = ALIGN(4);    
        *(.bss);
    } > sram
    .data :
    {
	. = ALIGN(4);    
        *(.data);
    } > sram
}

