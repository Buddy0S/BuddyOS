ENTRY(main);

SECTIONS 
{
    .text :
    {        
        . = ALIGN(4);
        build/usertest.o(.text.main);
        *(.text);
    }
    .rodata :
    {
	. = ALIGN(4);
        *(.rodata);
    }
    .bss :
    {
	. = ALIGN(4);    
        *(.bss);
    }
    .data :
    {
	. = ALIGN(4);    
        *(.data);
    }
}

