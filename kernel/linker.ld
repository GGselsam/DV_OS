OUTPUT_FORMAT("elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)

ENTRY(_start)

SECTIONS
{
    . = 0x10000;                /* Kernel loads at 64KB mark */

    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        __bss_start = .;
        *(.bss)
        __bss_end = .;
    }

    /DISCARD/ : {
        *(.eh_frame)
        *(.comment)
    }
}