; boot/stage2/kernel_handover.asm
; Robust handover mechanism from bootloader to kernel
[BITS 32]

; Structure to pass information to the kernel
struc boot_info
    .signature:     resd 1  ; Signature to verify boot info validity
    .memory_map:    resd 1  ; Pointer to memory map
    .memory_entries:resd 1  ; Number of memory map entries
    .boot_device:   resd 1  ; Boot device identifier
    .cmdline:       resd 1  ; Command line pointer
    .cmdline_size:  resd 1  ; Command line size
    .end:
endstruc

; Constants
BOOT_INFO_SIGNATURE equ 0x1BADB002  ; Boot info signature

; Function to prepare boot information for kernel
prepare_boot_info:
    ; Set up boot info structure
    mov edi, 0x9000  ; Address for boot info structure
    mov dword [edi + boot_info.signature], BOOT_INFO_SIGNATURE
    mov dword [edi + boot_info.memory_map], 0  ; Will be filled by memory detection
    mov dword [edi + boot_info.memory_entries], 0
    mov dword [edi + boot_info.boot_device], 0  ; Will be filled with boot device
    mov dword [edi + boot_info.cmdline], 0
    mov dword [edi + boot_info.cmdline_size], 0

    ; Store boot device
    mov al, [BOOT_DRIVE_ADDRESS]
    mov [edi + boot_info.boot_device], eax

    ; TODO: Add memory detection and command line parsing

    ret

; Function to perform handover to kernel
handover_to_kernel:
    ; Disable interrupts
    cli

    ; Prepare boot information
    call prepare_boot_info

    ; Set up registers for kernel entry
    ; RDI = Boot info structure address
    ; RSI = Magic number (0x1BADB002)
    mov rdi, 0x9000    ; Boot info address
    mov rsi, 0x1BADB002 ; Magic number

    ; Jump to kernel entry point
    ; The kernel should be loaded at 0x100000 (1MB)
    jmp 0x100000

; Error handling for kernel handover
handover_error:
    mov esi, msg_handover_error
    call print_string_pm
    jmp halt_system

; Data section
section .data
msg_handover_error db "ERROR: Kernel handover failed!", 0x0D, 0x0A, 0