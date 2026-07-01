[bits 64]
[extern kernel_main]

section .text
global _start

_start:
    ; Clear BSS section
    extern __bss_start
    extern __bss_end
    mov rdi, __bss_start
    mov rcx, __bss_end
    sub rcx, rdi
    xor rax, rax
    rep stosb

    ; Setup stack
    mov rsp, stack_top

    ; Call kernel main
    call kernel_main

    ; Halt if kernel_main returns
    cli
    hlt
    jmp $

section .bss
align 16
stack_bottom:
    resb 16384                 ; 16KB stack
stack_top: