
MEMORY
{
    ram : ORIGIN = 0x8000, LENGTH = 0x100000
}

SECTIONS
{
    .text : { *(.text*) } > ram
    .rodata : { *(.rodata*) } > ram
    .data : { *(.data*) } > ram
    .bss : { *(.bss*) } > ram
}
