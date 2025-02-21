MEMORY
{
  dram   (rwx)  : ORIGIN = 0x80000000
}

ENTRY(init);

SECTIONS 
{
    . = dram;
    . = ALIGN(32);
    __vector_load__ = .;
    .text :
    {    
	build/vector_table.o(.text);    
        . = ALIGN(4);
        build/kinit.o(.text);
        build/kernel.o(.text.main);
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

