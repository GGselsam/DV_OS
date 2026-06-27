[64 bits]
[extern kernel_main]

section .text
global _start

_start:
    ;clear BSS section
    extern __bss_start
    extern __bss_end
    mov rdi, __bss_start
    mov rcx, __bss_end
    sub rcx, rdi 
    xor rax, rax
    rep stosb

    ;setup stack
    mov rsp, stack_top
    ;call C
    call kernel_main

    ;halt if returns
    cli
    hlt
    jmp $

    section .bss
    align 16
    stack_bottom:
    resb 16384
stack_top: