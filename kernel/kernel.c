#include "include/stdint.h"
#include "include/stddef.h"
#include "include/stdio.h"
#include "include/string.h"
#include "include/idt.h"
#include "kernel.h"
#include "vga.h"
#include "serial.h"
#include "util.h"

/* Helper macro to check multiboot magic value */
#define CHECK_MULTIBOOT_MAGIC(x) ((x) == MULTIBOOT_MAGIC)

/* Handler for test interrupt */
void test_interrupt_handler(registers_t* regs) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Custom handler for interrupt %d called\n", regs->int_no);
    puts(buffer);
}

/* Trigger a test interrupt (Division by Zero) */
void trigger_test_interrupt(void) {
    /* Division by zero will trigger interrupt 0 */
    int a = 10;
    int b = 0;
    int c = a / b;  /* This will cause a divide by zero exception */
    
    /* This line is here just to avoid compiler warnings */
    snprintf((char*)&c, 1, "");
}

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
    
    /* Test our snprintf implementation */
    char buffer[128];
    int len = snprintf(buffer, sizeof(buffer), 
                      "Testing snprintf: int=%d, hex=0x%x, char=%c, str=%s, ptr=%p\n",
                      12345, 0xABCD, 'X', "test string", (void*)0xDEADBEEF);
    
    puts("==== Testing stdio functions ====\n");
    puts(buffer);
    snprintf(buffer, sizeof(buffer), "Format string length: %d\n", len);
    puts(buffer);
    
    /* Test string utility functions */
    puts("\n==== Testing string functions ====\n");
    
    /* Test strlen and strnlen */
    const char* test_str = "This is a test string";
    snprintf(buffer, sizeof(buffer), "strlen(\"%s\") = %d\n", test_str, strlen(test_str));
    puts(buffer);
    snprintf(buffer, sizeof(buffer), "strnlen(\"%s\", 10) = %d\n", test_str, strnlen(test_str, 10));
    puts(buffer);
    
    /* Test string copy */
    char dest[64];
    memset(dest, 0, sizeof(dest));
    strcpy(dest, "Hello");
    strncpy(dest + 5, " World!", 8);
    snprintf(buffer, sizeof(buffer), "After strcpy/strncpy: \"%s\"\n", dest);
    puts(buffer);
    
    /* Test string comparison */
    const char* str1 = "apple";
    const char* str2 = "apple";
    const char* str3 = "banana";
    snprintf(buffer, sizeof(buffer), "strcmp(\"%s\", \"%s\") = %d\n", str1, str2, strcmp(str1, str2));
    puts(buffer);
    snprintf(buffer, sizeof(buffer), "strcmp(\"%s\", \"%s\") = %d\n", str1, str3, strcmp(str1, str3));
    puts(buffer);
    snprintf(buffer, sizeof(buffer), "strncmp(\"%s\", \"%s\", 3) = %d\n", str1, str3, strncmp(str1, str3, 3));
    puts(buffer);
    
    /* Test memory functions */
    char mem1[16];
    char mem2[16];
    
    memset(mem1, 'A', sizeof(mem1));
    mem1[15] = 0;
    snprintf(buffer, sizeof(buffer), "After memset: \"%s\"\n", mem1);
    puts(buffer);
    
    memcpy(mem2, mem1, sizeof(mem2));
    mem2[7] = 0;
    snprintf(buffer, sizeof(buffer), "After memcpy: \"%s\"\n", mem2);
    puts(buffer);
    
    snprintf(buffer, sizeof(buffer), "memcmp result: %d\n", memcmp(mem1, mem2, 7));
    puts(buffer);
    
    puts("================================\n");
    
    /* Set up interrupt handling */
    puts("\n==== Setting up interrupt handling ====\n");
    
    /* Initialize the IDT */
    idt_init();
    
    /* Register a custom handler for the divide by zero exception */
    register_interrupt_handler(0, test_interrupt_handler);
    
    puts("Registered custom interrupt handler for divide by zero\n");
    puts("Triggering a divide by zero exception...\n");
    
    /* Trigger a test interrupt (Division by Zero) */
    trigger_test_interrupt();
    
    /* The code should continue here after handling the interrupt */
    puts("Returned from interrupt handler\n");
    puts("================================\n");
    
    /* Adding a small delay and extra output to make sure we see everything */
    delay(100);
    puts("Kernel is now halting...\n");
    
    /* This should prevent infinite reboot loops */
    while (1) {
        asm volatile ("cli");  /* Disable interrupts */
        asm volatile ("hlt");  /* Halt CPU */
    }
} 