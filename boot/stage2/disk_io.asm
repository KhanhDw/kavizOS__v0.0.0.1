; boot/stage2/disk_io.asm
; BITS 16
[BITS 16]
section .text

; Ghi chú:
; - BOOT_DRIVE_ADDRESS ở main file (0x7DF0)
; - Sử dụng CH/CL/DH/DL cho Cylinder/Sector/Head/Drive nếu dùng CHS.
;   Ở ví dụ này dùng CHS giả định (cylinder/head/sector) hoặc dùng LBA->CHS ngoài scope.
; - Bạn có thể thay đổi start sectors theo file image của bạn.

%define STAGE3_START_SECTOR 10
%define STAGE3_SECTORS      16

%define KERNEL_START_SECTOR 26
%define KERNEL_SECTORS      10   ; thay bằng số sector kernel thực tế

; ---------------------------------------------------------------------------
; Read sectors: thực hiện một lần int13 để đọc AL sectors, start sector in CX, head in DH
; ES:BX phải đặt trước khi gọi
; Input: AH=0x02, AL = sectors to read, CH = cylinder low, CL = sector, DH = head, DL = drive
; Return: CF clear on success, CF set on error and AH = error code
; ---------------------------------------------------------------------------

; Reset disk
reset_disk:
    mov ah, 0x00
    mov dl, [BOOT_DRIVE_ADDRESS]
    int 0x13
    jc .disk_reset_err
    ret
.disk_reset_err:
    mov si, msg_disk_reset_error
    call print_string
    jmp $

; load_stage3: đọc STAGE3 into ES:BX = 0x1000:0
load_stage3:
    call reset_disk
    mov dl, [BOOT_DRIVE_ADDRESS]

    ; set ES:BX => physical 0x10000
    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02         ; read sectors
    mov al, STAGE3_SECTORS
    ; CH/CL/DH must be set to the CHS of sector STAGE3_START_SECTOR
    ; Simple approach: if your image is linear and you know start sector, you can set CH=0, DH=0, CL=STAGE3_START_SECTOR
    mov ch, 0
    mov cl, STAGE3_START_SECTOR
    mov dh, 0
    int 0x13
    jc .load_error

    mov si, msg_stage3_loaded
    call print_string
    ret

.load_error:
    mov si, msg_load_error
    call print_string
    mov al, ah
    call print_hex
    jmp $

; load_kernel: đọc KERNEL vào ES:BX = 0x1000:0x1000 (phys 0x11000)
load_kernel:
    call reset_disk
    mov dl, [BOOT_DRIVE_ADDRESS]

    ; ES:BX = 0x1000:0x1000 -> phys 0x11000
    mov ax, 0x1000
    mov es, ax
    mov bx, 0x1000

    ; đọc KERNEL_SECTORS sectors bắt đầu từ KERNEL_START_SECTOR
    mov ah, 0x02
    mov al, KERNEL_SECTORS
    mov ch, 0
    mov cl, KERNEL_START_SECTOR
    mov dh, 0
    int 0x13
    jc .load_kernel_err

    ret

.load_kernel_err:
    mov si, msg_load_error
    call print_string
    mov al, ah
    call print_hex
    jmp $
