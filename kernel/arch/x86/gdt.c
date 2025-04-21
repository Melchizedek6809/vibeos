#include "gdt.h"
#include <string.h>

#define GDT_ENTRIES 3 // We need 3 entries: Null, Kernel Code, Kernel Data

// Define the GDT entries array
struct gdt_entry gdt_entries[GDT_ENTRIES];

// Define the GDT pointer structure
struct gdt_ptr   gdt_pointer;

// Helper function to set a GDT entry
void gdt_set_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    if (index >= GDT_ENTRIES) {
        // Handle error: index out of bounds
        // For now, we'll just return
        return;
    }

    // Set the base address parts
    gdt_entries[index].base_low    = (base & 0xFFFF);
    gdt_entries[index].base_middle = (base >> 16) & 0xFF;
    gdt_entries[index].base_high   = (base >> 24) & 0xFF;

    // Set the limit parts
    // Note: If granularity bit (G) is set, limit is in 4 KiB blocks
    gdt_entries[index].limit_low   = (limit & 0xFFFF);
    gdt_entries[index].granularity = ((limit >> 16) & 0x0F);

    // Set the granularity and access flags
    gdt_entries[index].granularity |= (granularity & 0xF0);
    gdt_entries[index].access      = access;
}

// Initialize the GDT
void gdt_init() {
    // Set up the GDT pointer
    gdt_pointer.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gdt_pointer.base  = (uint32_t)&gdt_entries;

    // Clear the GDT entries initially (good practice)
    memset(&gdt_entries, 0, sizeof(struct gdt_entry) * GDT_ENTRIES);

    // Set up the GDT entries
    // Entry 0: Null Segment (required)
    gdt_set_entry(0, 0, 0, 0, 0);

    // Entry 1: Kernel Code Segment (Ring 0)
    // Base=0, Limit=4GB (0xFFFFF pages), Access=0x9A, Granularity=0xCF
    // Access (0x9A): Present(1) DPL(00) Type(1=Code/Data) Code(1) Conforming(0) Readable(1) Accessed(0)
    // Granularity (0xCF): Granularity(1=4K) Size(1=32bit) 0 LimitHigh(F)
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xCF);

    // Entry 2: Kernel Data Segment (Ring 0)
    // Base=0, Limit=4GB (0xFFFFF pages), Access=0x92, Granularity=0xCF
    // Access (0x92): Present(1) DPL(00) Type(1=Code/Data) Data(0) Direction(0) Writable(1) Accessed(0)
    // Granularity (0xCF): Granularity(1=4K) Size(1=32bit) 0 LimitHigh(F)
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xCF);

    // Load the GDT
    gdt_load(&gdt_pointer);

    // Flush segment registers (important!)
    // Selector values: index * 8 (e.g., 1*8=0x08 for code, 2*8=0x10 for data)
    // The RPL (bits 0-1) should be 0 for kernel mode
    gdt_flush(0x08, 0x10);
} 