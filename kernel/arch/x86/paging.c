#include "paging.h"
#include <string.h>

// Declare the page directory and the first page table.
// These need to be page-aligned (4KB).
// We use static to keep them internal to this file.
// Placing them in .bss by default (if not initialized) or .data.
// NOTE: This is temporary! A proper memory manager should allocate these.
static page_directory_t kernel_page_directory __attribute__((aligned(PAGE_SIZE)));
static page_table_t first_page_table __attribute__((aligned(PAGE_SIZE)));

// Initialize paging
void paging_init() {
    // Clear the page directory and the first page table
    memset(&kernel_page_directory, 0, sizeof(page_directory_t));
    memset(&first_page_table, 0, sizeof(page_table_t));

    // Identity map the first 4MB using the first page table
    // We fill the first page table (1024 entries * 4KB = 4MB)
    for (int i = 0; i < 1024; i++) {
        uint32_t page_frame_addr = i * PAGE_SIZE;
        // Set flags: Present, Read/Write (kernel only for now)
        first_page_table.entries[i] = page_frame_addr | PTE_PRESENT | PTE_READ_WRITE;
    }

    // Put the first page table into the first entry of the page directory
    // Get the physical address of the page table
    uint32_t first_page_table_addr = (uint32_t)&first_page_table;
    // Set flags: Present, Read/Write (kernel only)
    kernel_page_directory.entries[0] = first_page_table_addr | PDE_PRESENT | PDE_READ_WRITE;

    // For simplicity, we can also map the rest of the directory entries 
    // to the same first page table, or leave them blank (not present).
    // Leaving them blank is safer as it prevents accidental access beyond 4MB.
    // If needed later, we can map more page tables.

    // Load the page directory address into CR3
    // Get the physical address of the page directory itself
    uint32_t page_directory_addr = (uint32_t)&kernel_page_directory;
    load_page_directory(page_directory_addr);

    // Enable paging by setting the PG bit in CR0
    enable_paging();
    
    // Paging is now enabled! Virtual addresses are now active.
    // Since we identity mapped, addresses 0x00000000 to 0x003FFFFF 
    // map to the same physical addresses.
} 