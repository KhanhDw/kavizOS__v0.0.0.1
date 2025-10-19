; boot/stage2/main_bootloader.asm
[BITS 16]
[ORG 0x7E00]

BOOT_DRIVE_ADDRESS equ 0x7DF0



stage2_start:
    mov ax, 0
    mov ss, ax
    mov sp, 0x7C00
    mov ds, ax
    mov es, ax

    mov si, msg_stage2
    call print_string

    call load_stage3
    call enable_a20
    call load_gdt

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp CODE_SEG:init_pm

[BITS 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov ebp, 0x90000
    mov esp, ebp

    mov esi, msg_pm_loaded
    call print_string_pm

    mov esi, msg_jumping
    call print_string_pm

    call delay
    jmp 0x10000

    mov esi, msg_returned
    call print_string_pm
    jmp $

%include "print_string.asm"
%include "print_hex.asm"
%include "disk_io.asm"
%include "enable_a20.asm"
%include "gdt.asm"
%include "delay.asm"
%include "print_string_pm.asm"
%include "data.asm"