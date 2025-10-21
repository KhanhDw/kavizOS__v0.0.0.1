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

    ; Validate entry - check for reasonable values
    ; Skip entries with zero length
    cmp dword [edi + e820_entry.length_low], 0
    jne .valid_entry
    cmp dword [edi + e820_entry.length_high], 0
    je .skip_entry

.valid_entry:
    ; Print memory entry for debugging
    pushad
    call print_memory_entry
    popad

    ; Increment entry count
    inc ecx

.skip_entry:
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

; Function to print a memory entry (for debugging)
print_memory_entry:
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
    ; Print memory detection start message
    mov esi, msg_memory_detect_start
    call print_string_pm

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
    ; Print memory detection complete message
    mov esi, msg_memory_detect_done
    call print_string_pm
    
    ; Print entry count
    mov eax, [0x9000 + boot_info.memory_entries]
    call print_hex_pm
    
    ; New line
    mov esi, msg_newline
    call print_string_pm
    
    ; Calculate and print total available RAM
    call calculate_total_ram
    
    ret

; Function to calculate total available RAM from E820 map
calculate_total_ram:
    ; Print calculating message
    mov esi, msg_calculating_ram
    call print_string_pm
    
    ; Initialize counters
    xor edi, edi  ; Total low 32 bits
    xor ebx, ebx  ; Total high 32 bits
    mov ecx, [0x9000 + boot_info.memory_entries]  ; Entry count
    mov esi, 0x8000  ; Memory map address

    ; Check if we have entries
    cmp ecx, 0
    je .no_entries

.calculate_loop:
    push ecx
    push esi
    
    ; Check if entry is RAM type
    cmp dword [esi + e820_entry.type], E820_RAM
    jne .next_entry
    
    ; Add length to total
    mov eax, [esi + e820_entry.length_low]
    add edi, eax
    mov eax, [esi + e820_entry.length_high]
    adc ebx, eax  ; Add with carry
    
.next_entry:
    ; Move to next entry
    add esi, e820_entry.end
    
    pop esi
    pop ecx
    loop .calculate_loop

.print_total:
    ; Print total RAM (high 32 bits)
    mov eax, ebx
    call print_hex_pm
    
    ; Print total RAM (low 32 bits)
    mov eax, edi
    call print_hex_pm
    
    ; Print bytes message
    mov esi, msg_bytes
    call print_string_pm
    
    ; Convert to MB for easier reading
    push edi
    push ebx
    
    ; For simplicity, just use low 32 bits for MB calculation
    shr edi, 20  ; Divide by 1MB (2^20)
    mov eax, edi
    call print_hex_pm
    
    mov esi, msg_megabytes
    call print_string_pm
    
    pop ebx
    pop edi
    
    ; New line
    mov esi, msg_newline
    call print_string_pm
    
    ret

.no_entries:
    mov esi, msg_no_entries
    call print_string_pm
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

; Data section
section .data
msg_memory_detect_start db "Detecting memory using E820...", 0
msg_memory_detect_done db "Memory entries detected: ", 0
msg_calculating_ram db "Total available RAM: 0x", 0
msg_bytes db " bytes (", 0
msg_megabytes db " MB)", 0
msg_mem_base db "Base: 0x", 0
msg_mem_length db " Length: 0x", 0
msg_mem_type db " Type: 0x", 0
msg_no_entries db "No memory entries found", 0
msg_newline db 0x0D, 0x0A, 0