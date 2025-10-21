// kernel/interrupt.h
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

// IDT entry structure for 64-bit
struct idt_entry {
    uint16_t offset_low;   // Lower 16 bits of handler function address
    uint16_t selector;     // Kernel segment selector
    uint8_t  ist;         // Interrupt stack table (bits 0-2), rest reserved
    uint8_t  type_attr;    // Type and attributes
    uint16_t offset_mid;   // Middle 16 bits of handler function address
    uint32_t offset_high;  // Higher 32 bits of handler function address
    uint32_t reserved;     // Reserved
} __attribute__((packed));

// IDT pointer structure for 64-bit
struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// Interrupt frame structure for 64-bit
struct interrupt_frame {
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
};

// Registers structure for 64-bit
struct registers {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, useresp, ss;
};

void idt_init(void);
void setup_idt(void);
void setup_pic(void);
void enable_interrupts(void);
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);
void isr_handler(struct registers regs);
void apic_isr_handler(struct registers regs);

#endif