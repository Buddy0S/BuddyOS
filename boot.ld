MEMORY
{
  sram   (rwx)  : ORIGIN = 0x402F0400, LENGTH = 0x0000FC00
  l3_ram (rwx)  : ORIGIN = 0x40300000, LENGTH = 0x0000FFFF
}

__sram_start__ = ORIGIN(sram);
__sram_size__ = LENGTH(sram);
__stack_start__ = ORIGIN(l3_ram) + LENGTH(l3_ram);
__stack_size__ = LENGTH(l3_ram);

SECTIONS 
{
    .data :
    {
        . = 0x0;
        LONG(__sram_start__);
        LONG(SIZEOF(.data) - 0x8);
        *(.text);
        *(.text*);
    }
}
