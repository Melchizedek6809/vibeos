/* Entry point - the first code to be executed */
ENTRY(_start)

/* Physical load address of the kernel */
SECTIONS
{
    /* Begin at 1MB - a common place for kernels to be loaded by bootloaders */
    . = 1M;

    /* Multiboot header first - as it needs to be within the first 8K */
    .text : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    /* Read-only data */
    .rodata : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Read-write data (initialized) */
    .data : ALIGN(4K)
    {
        *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }

    /* Remove sections we don't need */
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
} 