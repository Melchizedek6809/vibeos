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
    printf("Custom handler for interrupt %d called\n", regs->int_no);
}

/* Trigger a test interrupt (Division by Zero) */
void trigger_test_interrupt(void) {
    /* Division by zero will trigger interrupt 0 */
    int a = 10;
    int b = 0;
    int c = a / b;  /* This will cause a divide by zero exception */
    
    /* This line is here just to avoid compiler warnings */
    printf("%d", c);
}

/* The kernel main function */
void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_addr) {
    /* First thing: initialize terminal and serial for output */
    terminal_init();
    serial_init();
    
    /* Basic headers for both outputs */
    printf("=== Welcome to VibeOS ===\n");
    
    /* Check multiboot magic but don't fail if it's wrong */
    if (!CHECK_MULTIBOOT_MAGIC(multiboot_magic)) {
        printf("Warning: Not booted with multiboot (QEMU direct boot mode)\n");
    } else {
        printf("Multiboot-compliant boot confirmed\n");
    }
    
    /* Basic hello world output to both console and serial */
    printf("Hello, World!\n");
    printf("Kernel booted successfully\n");
    
    /* Test our printf implementation */
    printf("==== Testing stdio functions ====\n");
    printf("Testing printf: int=%d, hex=0x%x, char=%c, str=%s, ptr=%p\n",
           12345, 0xABCD, 'X', "test string", (void*)0xDEADBEEF);
    printf("Format string length: %d\n", 81);
    
    /* Test ANSI color escape sequences */
    printf("\n==== Testing ANSI Color Support ====\n");
    printf("Normal Text\n");
    printf("\033[31mRed Text\033[0m\n");
    printf("\033[32mGreen Text\033[0m\n");
    printf("\033[33mYellow Text\033[0m\n");
    printf("\033[34mBlue Text\033[0m\n");
    printf("\033[35mMagenta Text\033[0m\n");
    printf("\033[36mCyan Text\033[0m\n");
    printf("\033[37mWhite Text\033[0m\n");
    
    printf("\033[1mBold Text\033[0m\n");
    printf("\033[1;31mBold Red Text\033[0m\n");
    printf("\033[1;32mBold Green Text\033[0m\n");
    
    printf("\033[41mRed Background\033[0m\n");
    printf("\033[42mGreen Background\033[0m\n");
    printf("\033[31;42mRed Text on Green Background\033[0m\n");
    printf("\033[1;37;44mBold White Text on Blue Background\033[0m\n");
    
    /* Test string utility functions */
    printf("\n==== Testing string functions ====\n");
    
    /* Test strlen and strnlen */
    const char* test_str = "This is a test string";
    printf("strlen(\"%s\") = %d\n", test_str, strlen(test_str));
    printf("strnlen(\"%s\", 10) = %d\n", test_str, strnlen(test_str, 10));
    
    /* Test string copy */
    char dest[64];
    memset(dest, 0, sizeof(dest));
    strcpy(dest, "Hello");
    strncpy(dest + 5, " World!", 8);
    printf("After strcpy/strncpy: \"%s\"\n", dest);
    
    /* Test string comparison */
    const char* str1 = "apple";
    const char* str2 = "apple";
    const char* str3 = "banana";
    printf("strcmp(\"%s\", \"%s\") = %d\n", str1, str2, strcmp(str1, str2));
    printf("strcmp(\"%s\", \"%s\") = %d\n", str1, str3, strcmp(str1, str3));
    printf("strncmp(\"%s\", \"%s\", 3) = %d\n", str1, str3, strncmp(str1, str3, 3));
    
    /* Test memory functions */
    char mem1[16];
    char mem2[16];
    
    memset(mem1, 'A', sizeof(mem1));
    mem1[15] = 0;
    printf("After memset: \"%s\"\n", mem1);
    
    memcpy(mem2, mem1, sizeof(mem2));
    mem2[7] = 0;
    printf("After memcpy: \"%s\"\n", mem2);
    
    printf("memcmp result: %d\n", memcmp(mem1, mem2, 7));
    
    printf("================================\n");
    
    /* Set up interrupt handling */
    printf("\n==== Setting up interrupt handling ====\n");
    
    /* Initialize the IDT */
    idt_init();
    
    /* Register a custom handler for the divide by zero exception */
    register_interrupt_handler(0, test_interrupt_handler);
    
    printf("Registered custom interrupt handler for divide by zero\n");
    printf("Triggering a divide by zero exception...\n");
    
    /* Trigger a test interrupt (Division by Zero) */
    trigger_test_interrupt();
    
    /* The code should continue here after handling the interrupt */
    printf("Returned from interrupt handler\n");
    printf("================================\n");
    
    /* Adding a small delay and extra output to make sure we see everything */
    delay(100);
    printf("Kernel is now halting...\n");
    
    /* This should prevent infinite reboot loops */
    while (1) {
        asm volatile ("cli");  /* Disable interrupts */
        asm volatile ("hlt");  /* Halt CPU */
    }
} 