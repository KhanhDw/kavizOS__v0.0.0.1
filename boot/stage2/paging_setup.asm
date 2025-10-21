; boot/stage2/paging_setup.asm
; Proper paging setup for Long Mode (64-bit)
[BITS 32]

; Paging constants
PAGE_SIZE equ 4096
PML4_ADDR equ 0x100000    ; PML4 table at 1MB
PDPT_ADDR equ 0x101000    ; PDPT table at 1MB + 4KB
PD_ADDR   equ 0x102000    ; Page Directory at 1MB + 8KB
PT_ADDR   equ 0x103000    ; Page Table at 1MB + 12KB

; Page entry flags
PRESENT    equ 1 << 0
WRITABLE   equ 1 << 1
USER       equ 1 << 2
WRITE_THROUGH equ 1 << 3
CACHE_DISABLE equ 1 << 4
ACCESSED   equ 1 << 5
DIRTY      equ 1 << 6
HUGE_PAGE  equ 1 << 7
GLOBAL     equ 1 << 8
NO_EXECUTE equ 1 << 63

; Function to set up paging structures for Long Mode
setup_paging:
    ; Clear all paging structures
    call clear_paging_structures

    ; Set up PML4 (Page Map Level 4)
    call setup_pml4

    ; Set up PDPT (Page Directory Pointer Table)
    call setup_pdpt

    ; Set up PD (Page Directory)
    call setup_pd

    ; Set up PT (Page Table)
    call setup_pt

    ret

; Function to clear paging structures
clear_paging_structures:
    ; Clear PML4 (4KB)
    mov edi, PML4_ADDR
    mov ecx, PAGE_SIZE / 4
    xor eax, eax
    rep stosd

    ; Clear PDPT (4KB)
    mov edi, PDPT_ADDR
    mov ecx, PAGE_SIZE / 4
    xor eax, eax
    rep stosd

    ; Clear PD (4KB)
    mov edi, PD_ADDR
    mov ecx, PAGE_SIZE / 4
    xor eax, eax
    rep stosd

    ; Clear PT (4KB)
    mov edi, PT_ADDR
    mov ecx, PAGE_SIZE / 4
    xor eax, eax
    rep stosd

    ret

; Function to set up PML4
setup_pml4:
    mov edi, PML4_ADDR
    
    ; First entry: points to PDPT
    mov eax, PDPT_ADDR | PRESENT | WRITABLE
    mov [edi], eax
    
    ; Set other entries as needed for higher addresses
    ; For now, we'll just set up identity mapping for lower 4GB
    
    ret

; Function to set up PDPT
setup_pdpt:
    mov edi, PDPT_ADDR
    
    ; First entry: points to PD
    mov eax, PD_ADDR | PRESENT | WRITABLE
    mov [edi], eax
    
    ret

; Function to set up PD
setup_pd:
    mov edi, PD_ADDR
    
    ; Set up entries to point to page tables
    ; For identity mapping of first 2MB (512 entries * 4KB = 2MB)
    mov ecx, 512
    mov eax, PT_ADDR | PRESENT | WRITABLE  ; First PT
    
.loop:
    mov [edi], eax
    add eax, PAGE_SIZE  ; Point to next PT
    add edi, 8          ; Next entry (64-bit)
    loop .loop
    
    ret

; Function to set up PT
setup_pt:
    mov edi, PT_ADDR
    
    ; Set up identity mapping for first 2MB
    ; Each entry maps 4KB of physical memory
    mov ecx, 512        ; 512 entries * 4KB = 2MB
    xor eax, eax        ; Start at physical address 0
    or eax, PRESENT | WRITABLE
    
.loop:
    mov [edi], eax      ; Set page table entry
    add eax, PAGE_SIZE  ; Next 4KB page
    add edi, 4          ; Next entry (32-bit for PT)
    loop .loop
    
    ret

; Function to enable paging with PAE for Long Mode
enable_paging_long_mode:
    ; Load CR3 with PML4 address
    mov eax, PML4_ADDR
    mov cr3, eax

    ; Enable PAE (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5      ; Set PAE bit
    mov cr4, eax

    ; Enable Long Mode (EFER.LME = 1)
    mov ecx, 0xC0000080 ; EFER MSR
    rdmsr
    or eax, 1 << 8      ; Set LME bit
    wrmsr

    ; Enable Paging (CR0.PG = 1)
    mov eax, cr0
    or eax, 1 << 31     ; Set PG bit
    mov cr0, eax

    ret