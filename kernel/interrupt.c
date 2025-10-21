// kernel/interrupt.c
#include "interrupt.h"
#include "drivers/console.h"
#include "drivers/port_io.h"  // Include port I/O functions
#include "apic.h"
#include "timer.h"
#include <stdint.h>

// IDT entries array
struct idt_entry idt[256];

// IDT pointer
struct idt_ptr idtp;

// Function prototypes for ISRs
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

// PIC constants
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

// Exception messages
static const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// Remap PIC to avoid conflicts with CPU exceptions
void setup_pic(void) {
    // Save masks
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);
    
    // Start initialization sequence
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    
    // Set vector offsets
    outb(PIC1_DATA, 0x20);  // PIC1 -> 0x20 (32)
    outb(PIC2_DATA, 0x28);  // PIC2 -> 0x28 (40)
    
    // Continue initialization
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    
    // Set 8086 mode
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    
    // Restore masks
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
    
    // Disable all interrupts initially
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

// Set an IDT entry for 64-bit
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt[num].offset_low = base & 0xFFFF;
    idt[num].offset_mid = (base >> 16) & 0xFFFF;
    idt[num].offset_high = (base >> 32) & 0xFFFFFFFF;
    idt[num].selector = sel;
    idt[num].ist = 0;  // No IST
    idt[num].type_attr = flags;
    idt[num].reserved = 0;
}

// Install the IDT
void setup_idt(void) {
    // Set up IDT pointer
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint64_t)&idt;
    
    // Clear IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    // Set up exception handlers (ISR 0-31)
    idt_set_gate(0, (uint64_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint64_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint64_t)isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint64_t)isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint64_t)isr4, 0x08, 0x8E);
    idt_set_gate(5, (uint64_t)isr5, 0x08, 0x8E);
    idt_set_gate(6, (uint64_t)isr6, 0x08, 0x8E);
    idt_set_gate(7, (uint64_t)isr7, 0x08, 0x8E);
    idt_set_gate(8, (uint64_t)isr8, 0x08, 0x8E);
    idt_set_gate(9, (uint64_t)isr9, 0x08, 0x8E);
    idt_set_gate(10, (uint64_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint64_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint64_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint64_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint64_t)isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint64_t)isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint64_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint64_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint64_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint64_t)isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint64_t)isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint64_t)isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint64_t)isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint64_t)isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint64_t)isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint64_t)isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint64_t)isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint64_t)isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint64_t)isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint64_t)isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint64_t)isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint64_t)isr31, 0x08, 0x8E);
    
    // Set up APIC interrupt handlers (ISR 32-47)
    extern void apic_isr32(void);
    extern void apic_isr33(void);
    extern void apic_isr34(void);
    extern void apic_isr35(void);
    extern void apic_isr36(void);
    extern void apic_isr37(void);
    extern void apic_isr38(void);
    extern void apic_isr39(void);
    extern void apic_isr40(void);
    extern void apic_isr41(void);
    extern void apic_isr42(void);
    extern void apic_isr43(void);
    extern void apic_isr44(void);
    extern void apic_isr45(void);
    extern void apic_isr46(void);
    extern void apic_isr47(void);
    
    idt_set_gate(32, (uint64_t)apic_isr32, 0x08, 0x8E);
    idt_set_gate(33, (uint64_t)apic_isr33, 0x08, 0x8E);
    idt_set_gate(34, (uint64_t)apic_isr34, 0x08, 0x8E);
    idt_set_gate(35, (uint64_t)apic_isr35, 0x08, 0x8E);
    idt_set_gate(36, (uint64_t)apic_isr36, 0x08, 0x8E);
    idt_set_gate(37, (uint64_t)apic_isr37, 0x08, 0x8E);
    idt_set_gate(38, (uint64_t)apic_isr38, 0x08, 0x8E);
    idt_set_gate(39, (uint64_t)apic_isr39, 0x08, 0x8E);
    idt_set_gate(40, (uint64_t)apic_isr40, 0x08, 0x8E);
    idt_set_gate(41, (uint64_t)apic_isr41, 0x08, 0x8E);
    idt_set_gate(42, (uint64_t)apic_isr42, 0x08, 0x8E);
    idt_set_gate(43, (uint64_t)apic_isr43, 0x08, 0x8E);
    idt_set_gate(44, (uint64_t)apic_isr44, 0x08, 0x8E);
    idt_set_gate(45, (uint64_t)apic_isr45, 0x08, 0x8E);
    idt_set_gate(46, (uint64_t)apic_isr46, 0x08, 0x8E);
    idt_set_gate(47, (uint64_t)apic_isr47, 0x08, 0x8E);
    
    // Load the IDT
    asm volatile ("lidt %0" : : "m"(idtp));
}

// ISR handler in C
void isr_handler(struct registers regs) {
    // Handle exceptions
    if (regs.int_no < 32) {
        console_write("Exception: ");
        console_write(exception_messages[regs.int_no]);
        console_write("\nSystem Halted!\n");
        
        // Print error code if available
        if (regs.err_code != 0) {
            console_write("Error code: 0x");
            // Simple hex print (would need implementation)
            console_write("\n");
        }
        
        // Halt the system
        for (;;)
            asm volatile ("hlt");
    }
}

// APIC ISR handler in C
void apic_isr_handler(struct registers regs) {
    // Handle timer interrupt (IRQ 0)
    if (regs.int_no == 32) {
        // Call timer callback
        timer_callback();
    }
    // Handle other APIC interrupts
    else if (regs.int_no >= 32) {
        // For now, just send EOI to APIC
        apic_eoi();
    } else {
        // Handle exceptions through normal ISR handler
        isr_handler(regs);
    }
}

void enable_interrupts(void) {
    asm volatile ("sti"); // Enable interrupts
}

void idt_init(void) {
    setup_idt();
    setup_pic();
    
    // Initialize APIC
    apic_init();
    
    enable_interrupts();
}