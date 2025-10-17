; boot/main_bootloader.asm - stage 2
[BITS 16]
[ORG 0x7E00]

; Địa chỉ cố định để giao tiếp giữa boot sector và stage2
BOOT_DRIVE_ADDRESS equ 0x7DF0

;============================================
; PHẦN THỰC THI CHÍNH (16-BIT)
;============================================
stage2_start:
    ; Thiết lập lại segment
    mov ax, 0
    mov ss, ax
    mov sp, 0x7C00  ; Thiết lập stack an toàn

    mov ds, ax
    mov es, ax

    ; In thông báo
    mov si, msg_stage2
    call print_string

    ; Load stage3 từ disk
    call load_stage3

    ; Chuyển sang Protected Mode
    call enable_a20
    call load_gdt

    ; Vào Protected Mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Nhảy đến segment code 32-bit
    jmp CODE_SEG:init_pm

;============================================
; CÁC HÀM HỖ TRỢ (16-BIT)
;============================================

; Reset disk system
reset_disk:
    mov ah, 0x00
    mov dl, [BOOT_DRIVE_ADDRESS]
    int 0x13
    jc .disk_error
    ret
.disk_error:
    mov si, msg_disk_reset_error
    call print_string
    jmp $

; Load stage3 từ disk
load_stage3:
    call reset_disk

    mov dl, [BOOT_DRIVE_ADDRESS]

    mov ax, 0x0210        ; Đọc 16 sectors
    mov cx, 0x000A        ; Từ sector 10
    mov dh, 0
    mov bx, 0x1000
    mov es, bx
    mov bx, 0
    int 0x13
    jc .error

    cmp al, 16
    jne .error

    mov si, msg_stage3_loaded
    call print_string
    ret

.error:
    mov si, msg_load_error
    call print_string
    mov al, ah
    call print_hex
    jmp $

; Hàm in chuỗi 16-bit
print_string:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

; Hàm in số hex
print_hex:
    push ax
    mov ah, 0x0E
    mov cl, al
    shr cl, 4
    and cl, 0x0F
    call .print_digit
    mov cl, al
    and cl, 0x0F
    call .print_digit
    pop ax
    ret
.print_digit:
    cmp cl, 9
    jbe .digit
    add cl, 7
.digit:
    add cl, '0'
    mov al, cl
    int 0x10
    ret

; Kích hoạt A20 Line
enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret

; Load Global Descriptor Table
load_gdt:
    cli
    lgdt [gdt_descriptor]
    ret

;============================================
; PHẦN DỮ LIỆU (16-BIT)
;============================================

; Messages
msg_stage2 db "Stage2: Loading stage3...", 0x0D, 0x0A, 0
msg_stage3_loaded db "Stage3 loaded successfully!", 0x0D, 0x0A, 0
msg_load_error db "Stage3 load error! Code: ", 0
msg_disk_reset_error db "Disk reset error!", 0
msg_pm_loaded db "PM: Protected mode ready", 0
msg_jumping db "PM: Jumping to 0x10000", 0
msg_returned db "PM: ERROR: Returned from jump!", 0

; GDT
gdt_start:
    dq 0x0000000000000000
gdt_code:
    dw 0xFFFF, 0x0000
    db 0x00, 0x9A, 0xCF, 0x00
gdt_data:
    dw 0xFFFF, 0x0000
    db 0x00, 0x92, 0xCF, 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start


;============================================
; CHUYỂN SANG CHẾ ĐỘ 32-BIT
;============================================
[BITS 32]
init_pm:
    ; Thiết lập segment
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Thiết lập stack
    mov ebp, 0x90000
    mov esp, ebp

    ; In thông báo trước khi nhảy (dùng hàm 32-bit)
    mov esi, msg_pm_loaded
    call print_string_pm

    ; Nhảy đến Stage3 tại 0x10000
    mov esi, msg_jumping
    call print_string_pm

    call delay
    jmp 0x10000

    ; Nếu quay lại thì dừng
    mov esi, msg_returned
    call print_string_pm
    jmp $

; Hàm delay
delay:
    mov ecx, 0x00FFFFFF
.delay_loop:
    dec ecx
    jnz .delay_loop
    ret

; Hàm in chuỗi trong protected mode
print_string_pm:
    mov edi, 0xB8000 + 320  ; Dòng thứ 3 của màn hình
    mov ah, 0x0F            ; Màu trắng trên đen
.loop:
    lodsb
    test al, al
    jz .done
    mov [edi], ax
    add edi, 2
    jmp .loop
.done:
    ret

; Đảm bảo stage2 nằm trong 8 sectors (4096 bytes)
times 4096-($-$$) db 0