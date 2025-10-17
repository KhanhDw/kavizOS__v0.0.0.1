; kernel_core.asm - stage 3
; stage3.asm
[BITS 32]
[ORG 0x10000]

start:
    ; In thông báo
    mov esi, msg_stage3
    call print_string

    ; Tải kernel từ sector 26
    mov eax, 26        ; Sector bắt đầu
    mov ebx, 0x100000  ; Địa chỉ 1MB - nơi nạp kernel
    mov ecx, 10        ; Số sector cần đọc
    call load_sectors

    ; Nhảy đến kernel
    jmp 0x100000

load_sectors:
    ; Code đọc sector trong Protected Mode
    ; (cần implement)
    ret

print_string:
    ; In chuỗi trong Protected Mode
    ret

msg_stage3 db "Stage3: Loading kernel...",0