/* CMPT432 - ImplementationTeam00 */

MEMORY
{
  dram   (rwx)  : ORIGIN = 0x80000000, LENGTH = 512M
}

ENTRY(init);

SECTIONS 
{
    . = 0x80000000;
    .text :
    {        
        . = ALIGN(4);
        build/kinit.o(.text);
        build/kernel.o(.text.main);
        *(.text);
    } > dram
    .rodata :
    {
	. = ALIGN(4);
        *(.rodata);
    } > dram
    .bss :
    {
	. = ALIGN(4);    
        *(.bss);
    } > dram
    .data :
    {
	. = ALIGN(4);    
        *(.data);
    } > dram
}

