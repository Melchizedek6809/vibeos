#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "vga.h"
#include "serial.h"
#include "util.h"

/* Helper macro to check multiboot magic value */
#define CHECK_MULTIBOOT_MAGIC(x) ((x) == MULTIBOOT_MAGIC)

/* The kernel main function */
void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_addr) {
    /* First thing: initialize terminal and serial for output */
    terminal_init();
    serial_init();
    
    /* Basic headers for both outputs */
    puts("=== Welcome to VibeOS ===\n");
    
    /* Check multiboot magic but don't fail if it's wrong */
    if (!CHECK_MULTIBOOT_MAGIC(multiboot_magic)) {
        puts("Warning: Not booted with multiboot (QEMU direct boot mode)\n");
    } else {
        puts("Multiboot-compliant boot confirmed\n");
    }
    
    /* Basic hello world output to both console and serial */
    puts("Hello, World!\n");
    puts("Kernel booted successfully\n");
    puts("========================\n");
    
    /* Adding a small delay and extra output to make sure we see everything */
    delay(100);
    puts("Kernel is now halting...\n");
    
    /* This should prevent infinite reboot loops */
    while (1) {
        asm volatile ("cli");  /* Disable interrupts */
        asm volatile ("hlt");  /* Halt CPU */
    }
} 