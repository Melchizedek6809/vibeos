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
    terminal_write("=== Welcome to VibeOS ===\n");
    serial_write_string("=== Welcome to VibeOS ===\n");
    
    /* Check multiboot magic but don't fail if it's wrong */
    if (!CHECK_MULTIBOOT_MAGIC(multiboot_magic)) {
        terminal_write("Warning: Not booted with multiboot (QEMU direct boot mode)\n");
        serial_write_string("Warning: Not booted with multiboot (QEMU direct boot mode)\n");
    } else {
        terminal_write("Multiboot-compliant boot confirmed\n");
        serial_write_string("Multiboot-compliant boot confirmed\n");
    }
    
    /* Basic hello world output to both console and serial */
    terminal_write("Hello, World!\n");
    terminal_write("Kernel booted successfully\n");
    terminal_write("========================\n");
    
    serial_write_string("Hello, World!\n");
    serial_write_string("Kernel booted successfully\n");
    serial_write_string("========================\n");
    
    /* Adding a small delay and extra output to make sure we see everything */
    delay(100);
    serial_write_string("Kernel is now halting...\n");
    
    /* This should prevent infinite reboot loops */
    while (1) {
        asm volatile ("cli");  /* Disable interrupts */
        asm volatile ("hlt");  /* Halt CPU */
    }
} 