; boot_sector.asm - Stage 1 bootloader
[BITS 16]
[ORG 0x7C00]

BOOT_DRIVE_ADDRESS equ 0x7DF0

start:
    ; --- Thiết lập các segment ---
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; --- Lưu lại số hiệu ổ đĩa ---
    mov [BOOT_DRIVE_ADDRESS], dl

    ; --- In thông báo ---
    mov si, msg_loading
    call print_string

    ; --- Đọc stage2 (8 sectors từ sector 2) ---
    mov ax, 0x0208      ; 0x02 = đọc sector, 0x08 = 8 sector
    mov cx, 0x0002      ; bắt đầu từ sector 2
    mov dh, 0            ; đầu đọc 0
    mov bx, 0x7E00       ; nạp vào địa chỉ 0x7E00
    int 0x13
    jc disk_error        ; nếu lỗi → nhảy xử lý lỗi

    ; --- Nhảy đến stage2 ---
    jmp 0x0000:0x7E00

; --- Gọi xử lý lỗi ---
disk_error:
    call handle_disk_error

; --- Dữ liệu chuỗi ---
msg_loading db "Loading stage2...", 0
msg_disk_error db "Disk error!", 0

; --- Import các file con ---
%include "print_string.asm"
%include "disk_error.asm"

times 510-($-$$) db 0
dw 0xAA55
