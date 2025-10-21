; boot/stage2/bootloader_init.asm
; Comprehensive bootloader initialization
[BITS 32]

; Function to initialize all bootloader components
bootloader_init:
    ; Detect memory
    call detect_memory

    ; Set up interrupt handlers (if needed)
    ; call setup_idt

    ; Set up timer (if needed)
    ; call setup_timer

    ; Set up keyboard (if needed)
    ; call setup_keyboard

    ; Prepare for kernel handover
    call prepare_boot_info

    ret

; Function to print detailed boot information
print_boot_info:
    ; Print memory information
    mov esi, msg_memory_info
    call print_string_pm

    ; Print memory entry count
    mov eax, [0x9000 + boot_info.memory_entries]
    call print_hex_pm

    ; Print newline
    mov esi, msg_newline
    call print_string_pm

    ; Print detailed memory map if entries exist
    mov eax, [0x9000 + boot_info.memory_entries]
    cmp eax, 0
    je .no_entries

    ; Print each memory entry
    mov ecx, eax  ; Entry count
    mov edi, 0x8000  ; Memory map address

.print_loop:
    push ecx
    push edi
    
    ; Print base address
    mov esi, msg_mem_base
    call print_string_pm
    mov eax, [edi + e820_entry.base_high]
    call print_hex_pm
    mov eax, [edi + e820_entry.base_low]
    call print_hex_pm
    
    ; Print length
    mov esi, msg_mem_length
    call print_string_pm
    mov eax, [edi + e820_entry.length_high]
    call print_hex_pm
    mov eax, [edi + e820_entry.length_low]
    call print_hex_pm
    
    ; Print type
    mov esi, msg_mem_type
    call print_string_pm
    mov eax, [edi + e820_entry.type]
    call print_hex_pm
    
    ; New line
    mov esi, msg_newline
    call print_string_pm
    
    ; Move to next entry
    add edi, e820_entry.end
    
    pop edi
    pop ecx
    loop .print_loop

.no_entries:
    ret

; Function to validate kernel before handover
validate_kernel:
    ; Check if kernel is loaded at expected address
    ; TODO: Add kernel validation (checksum, signature, etc.)

    ret

; Main bootloader initialization function
main_init:
    ; Print initialization message
    mov esi, msg_init_start
    call print_string_pm

    ; Initialize bootloader components
    call bootloader_init

    ; Print boot information
    call print_boot_info

    ; Validate kernel
    call validate_kernel

    ; Print handover message
    mov esi, msg_handover_start
    call print_string_pm

    ; Perform handover to kernel
    call handover_to_kernel

    ; If we return here, there was an error
    jmp handover_error

; Data section
section .data
msg_init_start db "Initializing bootloader components...", 0
msg_memory_info db "Memory entries detected: ", 0
msg_handover_start db "Preparing for kernel handover...", 0
msg_mem_base db "Base: 0x", 0
msg_mem_length db " Length: 0x", 0
msg_mem_type db " Type: 0x", 0
msg_newline db 0x0D, 0x0A, 0