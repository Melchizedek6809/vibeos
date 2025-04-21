#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* Segment selectors */
#define KERNEL_CS 0x08 /* Kernel code segment */

/* Interrupt gate types */
#define IDT_GATE_TYPE_INTERRUPT 0x8E /* Present, DPL=0, Interrupt Gate */
#define IDT_GATE_TYPE_TRAP      0x8F /* Present, DPL=0, Trap Gate */

/* Number of ISRs in our IDT */
#define IDT_ENTRIES 256

/* Define the structure of an IDT entry */
typedef struct {
    uint16_t base_low;     /* Lower 16 bits of handler address */
    uint16_t selector;     /* Kernel segment selector */
    uint8_t  always0;      /* Always zero */
    uint8_t  flags;        /* Present, DPL, type flags */
    uint16_t base_high;    /* Upper 16 bits of handler address */
} __attribute__((packed)) idt_entry_t;

/* IDTR structure - points to the IDT */
typedef struct {
    uint16_t limit;        /* Size of IDT - 1 */
    uint32_t base;         /* Base address of IDT */
} __attribute__((packed)) idtr_t;

/* Initialize the IDT */
void idt_init(void);

/* Set an entry in the IDT */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

/* External assembly function to load the IDT register */
extern void idt_load(uint32_t idt_ptr);

/* Define a structure for interrupt frame */
typedef struct {
    uint32_t ds;                  /* Data segment selector */
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; /* Pushed by pusha */
    uint32_t int_no, err_code;    /* Interrupt number and error code */
    uint32_t eip, cs, eflags, useresp, ss; /* Pushed by the processor automatically */
} registers_t;

/* Function pointer type for interrupt handlers */
typedef void (*isr_handler_t)(registers_t*);

/* Register a custom interrupt handler function */
extern void register_interrupt_handler(uint8_t n, isr_handler_t handler);

/* The main interrupt handler function */
extern void isr_handler(registers_t regs);

#endif /* IDT_H */ 