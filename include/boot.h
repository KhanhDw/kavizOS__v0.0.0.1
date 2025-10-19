  /* include/boot.h */
#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>

/* Memory addresses */
#define VIDEO_MEMORY 0xB8000
#define KERNEL_LOAD_ADDR 0x100000

/* Boot parameters structure */
struct boot_params {
    uint8_t setup_sects;      // số sector stage setup
    uint16_t root_flags;
    uint32_t sys_size;
    uint16_t vid_mode;
    uint16_t root_dev;
    uint32_t mem_lower;       // dưới 1MB
    uint32_t mem_upper;       // trên 1MB
    uint8_t  boot_drive;      // ổ đĩa boot
    uint8_t  boot_sector;
    uint32_t kernel_addr;
} __attribute__((packed));


/* Function prototypes */
void clear_screen(void);
void print_string32(const char *str);
void load_kernel(void);
void setup_boot_params(void);
void jump_to_kernel(void);


#endif