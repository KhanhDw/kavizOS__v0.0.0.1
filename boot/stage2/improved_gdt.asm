; boot/stage2/improved_gdt.asm
; Improved GDT with proper descriptors for both 32-bit and 64-bit modes
[BITS 16]

gdt_start:
    ; Null descriptor
    dq 0x0000000000000000
    
    ; 32-bit code segment descriptor
    ; Base = 0x00000000, Limit = 0xFFFFFFFF
    ; Present, DPL=0, Code, Read/Execute, Conforming, 4KB Granularity, 32-bit Default
gdt_code32:
    dw 0xFFFF        ; Limit (low 16 bits)
    dw 0x0000        ; Base (low 16 bits)
    db 0x00          ; Base (middle 8 bits)
    db 10011010b     ; Present, DPL=0, Code, Read/Execute
    db 11001111b     ; 4KB Granularity, 32-bit Default, Limit (high 4 bits)
    db 0x00          ; Base (high 8 bits)
    
    ; 32-bit data segment descriptor
    ; Base = 0x00000000, Limit = 0xFFFFFFFF
    ; Present, DPL=0, Data, Read/Write, Expand-up, 4KB Granularity, 32-bit
gdt_data32:
    dw 0xFFFF        ; Limit (low 16 bits)
    dw 0x0000        ; Base (low 16 bits)
    db 0x00          ; Base (middle 8 bits)
    db 10010010b     ; Present, DPL=0, Data, Read/Write
    db 11001111b     ; 4KB Granularity, 32-bit, Limit (high 4 bits)
    db 0x00          ; Base (high 8 bits)
    
    ; 64-bit code segment descriptor (for Long Mode)
    ; Base = 0x00000000, Limit = 0x000FFFFF
    ; Present, DPL=0, Code, Read/Execute, Conforming, 4KB Granularity, 64-bit
gdt_code64:
    dw 0xFFFF        ; Limit (low 16 bits)
    dw 0x0000        ; Base (low 16 bits)
    db 0x00          ; Base (middle 8 bits)
    db 10011010b     ; Present, DPL=0, Code, Read/Execute
    db 10101111b     ; 4KB Granularity, 64-bit, Limit (high 4 bits)
    db 0x00          ; Base (high 8 bits)
    
    ; 64-bit data segment descriptor
    ; Base = 0x00000000, Limit = 0x000FFFFF
    ; Present, DPL=0, Data, Read/Write, Expand-up, 4KB Granularity
gdt_data64:
    dw 0xFFFF        ; Limit (low 16 bits)
    dw 0x0000        ; Base (low 16 bits)
    db 0x00          ; Base (middle 8 bits)
    db 10010010b     ; Present, DPL=0, Data, Read/Write
    db 10101111b     ; 4KB Granularity, 64-bit, Limit (high 4 bits)
    db 0x00          ; Base (high 8 bits)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT (limit)
    dd gdt_start                ; Address of GDT

; Segment selectors
CODE32_SEG equ gdt_code32 - gdt_start
DATA32_SEG equ gdt_data32 - gdt_start
CODE64_SEG equ gdt_code64 - gdt_start
DATA64_SEG equ gdt_data64 - gdt_start

; Function to load GDT
load_gdt:
    cli
    lgdt [gdt_descriptor]
    ret