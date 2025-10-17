// kernel/interrupt.c
#include "interrupt.h"

// Hàm giả lập tạm thời - sẽ implement sau
void setup_idt(void) {
    // TODO: Thiết lập Interrupt Descriptor Table
}

void setup_pic(void) {
    // TODO: Thiết lập Programmable Interrupt Controller
}

void enable_interrupts(void) {
    asm volatile ("sti"); // Enable interrupts
}

void idt_init(void) {
    setup_idt();
    setup_pic();
    enable_interrupts();
}