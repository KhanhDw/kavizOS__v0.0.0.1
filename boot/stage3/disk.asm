; boot/stage3/disk.asm
[BITS 32]

section .text
global load_sectors

; Hàm đọc sector trong protected mode (sẽ cần switch về real mode)
; Hiện tại để trống vì phức tạp
load_sectors:
    ; TODO: Implement protected mode disk reading
    ; Có thể cần:
    ; 1. Quay lại real mode tạm thời
    ; 2. Sử dụng V86 mode
    ; 3. Sử dụng driver PCI/ATA trực tiếp
    ret