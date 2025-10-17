// kernel/drivers/console.h
#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>
#include <stdint.h>

struct console {
    size_t row;
    size_t column;
    uint8_t color;
    uint16_t* buffer;
};

// Hàm public
void console_initialize(void);
void console_putchar(char c);
void console_write(const char* data);
void console_clear(void);
void console_scroll(void);                    // THÊM DÒNG NÀY
void console_update_cursor(void);             // THÊM DÒNG NÀY

#endif