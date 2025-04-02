MEMORY
{
  dram   (rwx)  : ORIGIN = 0x85000000, LENGTH = 512M
}

ENTRY(main);

SECTIONS 
{
    . = 0x85000000;
    .text :
    {        
        . = ALIGN(4);
        build/schat.o(.text.main);
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

