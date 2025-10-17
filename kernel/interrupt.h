// kernel/interrupt.h
#ifndef INTERRUPT_H
#define INTERRUPT_H

void idt_init(void);
void setup_idt(void);
void setup_pic(void);
void enable_interrupts(void);

#endif