; boot/stage2/improved_data.asm
[BITS 16]

; Success messages
msg_stage2 db "Stage2: Loading stage3...", 0x0D, 0x0A, 0
msg_stage3_loaded db "Stage3 loaded successfully!", 0x0D, 0x0A, 0
msg_pm_loaded db "PM: Protected mode ready", 0
msg_jumping db "PM: Jumping to 0x10000", 0

; Error messages
msg_load_error db "ERROR: Stage3 load failed!", 0x0D, 0x0A, 0
msg_a20_error db "ERROR: A20 line enable failed!", 0x0D, 0x0A, 0
msg_gdt_error db "ERROR: GDT load failed!", 0x0D, 0x0A, 0
msg_pm_error db "ERROR: Protected mode transition failed!", 0x0D, 0x0A, 0
msg_returned db "ERROR: Returned from jump - kernel failed!", 0x0D, 0x0A, 0