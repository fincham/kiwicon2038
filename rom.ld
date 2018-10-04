OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 0x3;
    .text :
    {
        *(.text);
    }
    .data :
    {
        *(.data);
        *(.bss);
        *(.rodata);
    }
    _heap = ALIGN(4);
}
