#ifndef KERNEL_GDT_H
#define KERNEL_GDT_H

#include <stdint.h>

// Structure for a GDT entry
// Packed attribute prevents compiler padding
struct gdt_entry {
    uint16_t limit_low;    // Lower 16 bits of the limit
    uint16_t base_low;     // Lower 16 bits of the base
    uint8_t  base_middle;  // Next 8 bits of the base
    uint8_t  access;       // Access flags, determine ring level
    uint8_t  granularity;  // Granularity, limit_high (4 bits), flags (4 bits)
    uint8_t  base_high;    // Last 8 bits of the base
} __attribute__((packed));

// Structure for the GDT pointer (used by lgdt instruction)
struct gdt_ptr {
    uint16_t limit;   // Size of GDT - 1
    uint32_t base;    // Address of the first GDT entry
} __attribute__((packed));

// Function to initialize the GDT
void gdt_init();

// External assembly function to load the GDT
// Defined in gdt_load.asm
extern void gdt_load(struct gdt_ptr* gdt_ptr_addr);

// External assembly function to flush segment registers after GDT load
// Defined in gdt_load.asm
extern void gdt_flush(uint16_t code_selector, uint16_t data_selector);

#endif // KERNEL_GDT_H 