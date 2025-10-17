// kernel/kernel_loader.c
#include "boot.h"
#include "drivers/console.h"

void kernel_main(void) {
    // Khởi tạo console
    console_initialize();

    // In thông báo khởi động
    console_write("MyOS Kernel Booted Successfully!\n");
    console_write("Console System: READY\n");

    // Test các chức năng console
    console_write("Testing console functions:\n");
    console_write("-> Line 1\n");
    console_write("-> Line 2\n");
    console_write("-> Line 3\n");

    console_write("> ");

    // Vòng lặp chính
    while (1) {
        asm volatile ("hlt");
    }
}