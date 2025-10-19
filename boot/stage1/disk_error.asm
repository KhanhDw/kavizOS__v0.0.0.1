; disk_error.asm - Xử lý lỗi khi đọc đĩa
[BITS 16]

global handle_disk_error


handle_disk_error:
    mov si, msg_disk_error
    call print_string
    jmp $
