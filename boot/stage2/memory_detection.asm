; boot/stage2/memory_detection.asm
; Memory detection for the bootloader
[BITS 32]

; E820 memory map entry structure
struc e820_entry
    .base_low:   resd 1    ; Base address low 32 bits
    .base_high:  resd 1    ; Base address high 32 bits
    .length_low: resd 1    ; Length low 32 bits
    .length_high:resd 1    ; Length high 32 bits
    .type:       resd 1    ; Memory type
    .acpi:       resd 1    ; ACPI 3.0 fields
    .end:
endstruc

; Memory types
E820_RAM     equ 1
E820_RESERVED equ 2
E820_ACPI    equ 3
E820_NVS     equ 4
E820_UNUSABLE equ 5

; Function to detect memory using E820 BIOS call
detect_memory_e820:
    ; Set up buffer for memory map entries
    mov edi, 0x8000  ; Buffer address for memory map
    xor ebx, ebx         ; Continuation value (0 for first call)
    mov ecx, 0       ; Entry count
    mov edx, 0x534D4150  ; Signature "SMAP"

.e820_loop:
    ; Set up registers for INT 15h, EAX=E820h
    mov eax, 0xE820
    mov ecx, 24          ; Size of buffer
    int 0x15

    ; Check if call was successful
    jc .e820_done         ; Carry set = error

    ; Check signature
    cmp edx, 0x534D4150  ; "SMAP"
    jne .e820_done

    ; Increment entry count
    inc ecx

    ; Move to next entry
    add edi, e820_entry.end

    ; Check if more entries
    test ebx, ebx
    jnz .e820_loop

.e820_done:
    ; Store entry count
    mov [0x9000 + boot_info.memory_entries], ecx

    ; Store memory map address
    mov dword [0x9000 + boot_info.memory_map], 0x8000

    ret

; Function to detect memory using EAX=E881h (fallback)
detect_memory_e881:
    ; TODO: Implement E881 memory detection
    ret

; Function to detect memory using EAX=E801h (fallback)
detect_memory_e801:
    ; TODO: Implement E801 memory detection
    ret

; Main memory detection function
detect_memory:
    ; Try E820 first (most modern)
    call detect_memory_e820
    cmp dword [0x9000 + boot_info.memory_entries], 0
    jne .memory_detected

    ; Try E881 (fallback)
    call detect_memory_e881
    cmp dword [0x9000 + boot_info.memory_entries], 0
    jne .memory_detected

    ; Try E801 (fallback)
    call detect_memory_e801
    cmp dword [0x9000 + boot_info.memory_entries], 0
    jne .memory_detected

    ; If all methods fail, use a simple detection
    call simple_memory_detection

.memory_detected:
    ret

; Simple memory detection (fallback)
simple_memory_detection:
    ; For now, just set a basic memory map
    mov edi, 0x8000
    mov dword [edi + e820_entry.base_low], 0
    mov dword [edi + e820_entry.base_high], 0
    mov dword [edi + e820_entry.length_low], 0x100000  ; First 1MB
    mov dword [edi + e820_entry.length_high], 0
    mov dword [edi + e820_entry.type], E820_RAM

    ; Second entry: 1MB to 128MB
    mov edi, 0x8000 + e820_entry.end
    mov dword [edi + e820_entry.base_low], 0x100000
    mov dword [edi + e820_entry.base_high], 0
    mov dword [edi + e820_entry.length_low], 0x7F00000  ; 127MB
    mov dword [edi + e820_entry.length_high], 0
    mov dword [edi + e820_entry.type], E820_RAM

    ; Set entry count
    mov dword [0x9000 + boot_info.memory_entries], 2
    mov dword [0x9000 + boot_info.memory_map], 0x8000

    ret