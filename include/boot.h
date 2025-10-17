  /* include/boot.h */
#ifndef BOOT_H
#define BOOT_H

#include <stdint.h>

/* Memory addresses */
#define VIDEO_MEMORY 0xB8000
#define KERNEL_LOAD_ADDR 0x100000

/* Boot parameters structure */
struct boot_params {
    uint8_t setup_sects;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint8_t video_mode;
    // ... thêm các trường khác theo chuẩn Linux
} __attribute__((packed));

/* Function prototypes */
void clear_screen(void);
void print_string32(const char *str);
void load_kernel(void);
void setup_boot_params(void);
void jump_to_kernel(void);

#endif