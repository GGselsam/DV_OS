[bits 16]
[org 0x7c00]

start:
    ; Disable interrupts and setup segments
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Clear screen
    mov ax, 0x0003
    int 0x10

    ; Print loading message
    mov si, msg_loading
    call print_string_16

    ; Load kernel from disk
    call load_kernel
    ; Switch to long mode
    call switch_to_long_mode

    ; Jump to kernel (address 0x100000)
    jmp 0x0000:0x100000

print_string_16:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

load_kernel:
    mov si, msg_loading_kernel
    call print_string_16

    ; Load kernel from disk to 0x100000
    ; Address = 0x1000:0x0000 = 0x100000
    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    ; BIOS read sectors function
    mov ah, 0x02
    mov al, 30          ; Number of sectors to read
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Sector 2 (after bootloader)
    mov dh, 0           ; Head 0
    mov dl, 0x80        ; Drive C:
    int 0x13

    jc disk_error
    ret

disk_error:
    mov si, msg_disk_error
    call print_string_16
    jmp $

switch_to_long_mode:
    ; Clear page tables (16KB)
    mov edi, 0x1000
    mov cr3, edi
    xor eax, eax
    mov ecx, 4096
    rep stosd

    ; Setup PML4 (0x1000) -> PDPT (0x2000)
    mov edi, 0x1000
    mov dword [edi], 0x2003

    ; Setup PDPT (0x2000) -> PDT (0x3000)
    mov edi, 0x2000
    mov dword [edi], 0x3003

    ; Setup PDT (0x3000) -> PT (0x4000)
    mov edi, 0x3000
    mov dword [edi], 0x4003

    ; Fill PT (0x4000) - 512 entries of 4KB = 2MB
    mov edi, 0x4000
    mov ebx, 0x00000003
    mov ecx, 512
.set_pt:
    mov dword [edi], ebx
    mov dword [edi+4], 0
    add edi, 8
    add ebx, 0x1000
    loop .set_pt

    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Enable LME (Long Mode Enable) in MSR
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable protected mode and paging
    mov eax, cr0
    or eax, (1 << 31) | (1 << 0)
    mov cr0, eax

    ; Load GDT
    lgdt [gdt_descriptor]

    ; Far jump to long mode
    push 0x08
    push .long_mode_entry
    retf

[bits 64]
.long_mode_entry:
    ; Setup data segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; Clear screen in 64-bit mode
    mov edi, 0xB8000
    mov ecx, 80 * 25
    mov ax, 0x0F20      ; Space with white on black attribute
    rep stosw

    ; Print long mode message
    mov edi, 0xB8000
    mov rsi, msg_long_mode
    mov ah, 0x0F
.print:
    lodsb
    test al, al
    jz .done
    stosw
    jmp .print
.done:

    ret

gdt_64:
    dq 0                     ; Null descriptor
.code:
    dq 0x00209A0000000000    ; 64-bit code segment
.data:
    dq 0x0000920000000000    ; 64-bit data segment

gdt_descriptor:
    dw $ - gdt_64 - 1
    dq gdt_64

msg_loading db "DV OS: Loading...", 0x0D, 0x0A, 0
msg_loading_kernel db "Loading kernel from disk...", 0x0D, 0x0A, 0
msg_disk_error db "ERROR: Failed to load kernel!", 0x0D, 0x0A, 0
msg_long_mode db "DV OS: Entered 64-bit Long Mode!", 0

times 510-($-$$) db 0
dw 0xAA55
; i'm fucking it's launguarge