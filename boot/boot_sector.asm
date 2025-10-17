; boot/boot_sector.asm - stage 1
[BITS 16]
[ORG 0x7C00]

; --- FIX: Định nghĩa một địa chỉ cố định để lưu số hiệu ổ đĩa ---
BOOT_DRIVE_ADDRESS equ 0x7DF0

start:
    ; Thiết lập segment
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; --- FIX: Lưu số hiệu ổ đĩa vào địa chỉ cố định ---
    mov [BOOT_DRIVE_ADDRESS], dl

    ; In thông báo
    mov si, msg_loading
    call print_string

    ; Tải stage2 từ disk (8 sectors = 4KB)
    mov ax, 0x0208
    mov cx, 0x0002
    mov dh, 0
    mov bx, 0x7E00
    int 0x13
    jc disk_error

    jmp 0x0000:0x7E00

disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $

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

msg_loading db "Loading stage2...", 0
msg_disk_error db "Disk error!", 0

; --- FIX: Xóa biến boot_drive ở đây ---

times 510-($-$$) db 0
dw 0xAA55