; boot/stage2/improved_main_bootloader.asm
[BITS 16]
[ORG 0x7E00]

BOOT_DRIVE_ADDRESS equ 0x7DF0

stage2_start:
    ; Initialize segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Print stage2 loading message
    mov si, msg_stage2
    call print_string

    ; Load stage3 from disk
    call load_stage3
    
    ; Check for errors
    jc stage3_load_error

    ; Enable A20 line
    call enable_a20
    
    ; Check for errors
    jc a20_enable_error

    ; Load Global Descriptor Table
    call load_gdt

    ; Transition to Protected Mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Far jump to Protected Mode code
    jmp CODE32_SEG:init_pm

[BITS 32]
init_pm:
    ; Initialize segment registers for Protected Mode
    mov ax, DATA32_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up stack
    mov ebp, 0x90000
    mov esp, ebp

    ; Print Protected Mode ready message
    mov esi, msg_pm_loaded
    call print_string_pm

    ; Print jumping message
    mov esi, msg_jumping
    call print_string_pm

    ; Add a small delay
    call delay

    ; Jump to stage3 at 0x10000
    jmp 0x10000

    ; If we return here, there was an error
    mov esi, msg_returned
    call print_string_pm
    jmp $

; Error handlers
stage3_load_error:
    mov si, msg_load_error
    call print_string
    jmp halt_system

a20_enable_error:
    mov si, msg_a20_error
    call print_string
    jmp halt_system

halt_system:
    cli
    hlt
    jmp halt_system

%include "print_string.asm"
%include "print_hex.asm"
%include "disk_io.asm"
%include "enable_a20.asm"
%include "improved_gdt.asm"
%include "delay.asm"
%include "print_string_pm.asm"
%include "improved_data.asm"