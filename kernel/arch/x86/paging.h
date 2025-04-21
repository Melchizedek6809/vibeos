#ifndef KERNEL_PAGING_H
#define KERNEL_PAGING_H

#include "../../include/stdint.h"
#include "../../include/stddef.h" // For NULL

#define PAGE_SIZE 4096

// Page Directory Entry Flags
#define PDE_PRESENT     (1 << 0) // Present bit
#define PDE_READ_WRITE  (1 << 1) // Read/Write bit
#define PDE_USER        (1 << 2) // User/Supervisor bit (0=Supervisor)
#define PDE_WRITE_THROUGH (1 << 3) // Write-Through caching
#define PDE_CACHE_DISABLE (1 << 4) // Cache Disabled
#define PDE_ACCESSED    (1 << 5) // Accessed bit (set by CPU)
#define PDE_DIRTY       (1 << 6) // Dirty bit (set by CPU on write)
#define PDE_SIZE_4MB    (1 << 7) // Page Size bit (0=4KB, 1=4MB)
#define PDE_GLOBAL      (1 << 8) // Global bit (ignored for PDEs? Check Intel docs)

// Page Table Entry Flags
#define PTE_PRESENT     (1 << 0) // Present bit
#define PTE_READ_WRITE  (1 << 1) // Read/Write bit
#define PTE_USER        (1 << 2) // User/Supervisor bit (0=Supervisor)
#define PTE_WRITE_THROUGH (1 << 3) // Write-Through caching
#define PTE_CACHE_DISABLE (1 << 4) // Cache Disabled
#define PTE_ACCESSED    (1 << 5) // Accessed bit
#define PTE_DIRTY       (1 << 6) // Dirty bit (set by CPU on write)
#define PTE_PAT         (1 << 7) // Page Attribute Table index
#define PTE_GLOBAL      (1 << 8) // Global page (prevents TLB flush on CR3 write if CR4.PGE=1)

// Structure for a Page Table Entry (PTE) - represents a 4KB page frame
typedef uint32_t page_table_entry_t;

// Structure for a Page Directory Entry (PDE) - can point to a Page Table or map a 4MB page
typedef uint32_t page_directory_entry_t;

// Structure for a Page Table (contains 1024 PTEs)
// Must be page-aligned (4KB)
typedef struct {
    page_table_entry_t entries[1024];
} __attribute__((aligned(PAGE_SIZE))) page_table_t;

// Structure for a Page Directory (contains 1024 PDEs)
// Must be page-aligned (4KB)
typedef struct {
    page_directory_entry_t entries[1024];
} __attribute__((aligned(PAGE_SIZE))) page_directory_t;

// Function to initialize basic paging
void paging_init();

// External assembly functions (defined in paging_enable.S)
extern void load_page_directory(uint32_t page_directory_addr);
extern void enable_paging();

#endif // KERNEL_PAGING_H 