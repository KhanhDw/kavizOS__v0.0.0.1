// kernel/drivers/console.c
#include "./vga.h"
#include "./console.h"
#include "./port_io.h"
#include <stdint.h>

// Biến toàn cục cho console
static struct console con;

void console_initialize(void) {
    con.row = 0;
    con.column = 0;
    con.color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    con.buffer = VGA_MEMORY;

    // Xóa màn hình
    console_clear();
}

void console_clear(void) {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            con.buffer[index] = vga_entry(' ', con.color);
        }
    }
    con.row = 0;
    con.column = 0;
    console_update_cursor();
}

// kernel/drivers/console.c

void console_putchar(char c) {
    // Xử lý ký tự newline
    if (c == '\n') {
        con.column = 0;
        con.row++;
    }
    // Xử lý ký tự carriage return
    else if (c == '\r') {
        con.column = 0;
    }
    // Xử lý ký tự backspace (đơn giản)
    else if (c == '\b') {
        if (con.column > 0) {
            con.column--;
            console_putchar(' '); // Ghi đè bằng khoảng trắng
            con.column--; // Lùi lại lần nữa
        }
    }
    // Ký tự in được bình thường
    else {
        const size_t index = con.row * VGA_WIDTH + con.column;
        con.buffer[index] = vga_entry(c, con.color);
        con.column++;
    }

    // Xử lý cuộn màn hình nếu cần
    if (con.column >= VGA_WIDTH) {
        con.column = 0;
        con.row++;
    }

    // Cuộn màn hình nếu vượt quá chiều cao
    if (con.row >= VGA_HEIGHT) {
        console_scroll();
    }

    // Cập nhật vị trí con trỏ
    console_update_cursor();
}


// kernel/drivers/console.c
void console_write(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        console_putchar(data[i]);
    }
}


// kernel/drivers/console.c

void console_scroll(void) {
    // Di chuyển tất cả các dòng lên một dòng
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            const size_t index_from = y * VGA_WIDTH + x;
            const size_t index_to = (y - 1) * VGA_WIDTH + x;
            con.buffer[index_to] = con.buffer[index_from];
        }
    }

    // Xóa dòng cuối cùng
    for (int x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        con.buffer[index] = vga_entry(' ', con.color);
    }

    con.row = VGA_HEIGHT - 1;
}


void console_update_cursor(void) {
    uint16_t pos = con.row * VGA_WIDTH + con.column;

    // Gửi lệnh đến VGA control register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}