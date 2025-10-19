; boot/stage2/data.asm
[BITS 16]

msg_stage2 db "Stage2: Loading stage3...", 0x0D, 0x0A, 0
msg_stage3_loaded db "Stage3 loaded successfully!", 0x0D, 0x0A, 0
msg_load_error db "Stage3 load error! Code: ", 0
msg_disk_reset_error db "Disk reset error!", 0
msg_pm_loaded db "PM: Protected mode ready", 0
msg_jumping db "PM: Jumping to 0x10000", 0
msg_returned db "PM: ERROR: Returned from jump!", 0
