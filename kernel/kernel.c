#include "include/stdint.h"
#include "include/stddef.h"
#include "include/stdio.h"
#include "include/string.h"
#include "include/idt.h"
#include "kernel.h"
#include "vga.h"
#include "serial.h"
#include "util.h"
#include "keyboard.h"

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

/* ASCII art logo for VibeOS */
void print_logo(void) {
    printf("\n");
    printf(" \033[1;31m__      __\033[1;33m \033[1;33m_          \033[1;36m_____\033[1;34m  _____\033[0m\n");
    printf(" \033[1;31m\\ \\    / /\033[1;33m(_)\033[1;32m        \033[1;36m/ ____|\033[1;34m/ ____|  \033[0m\n");
    printf(" \033[1;31m \\ \\  / / \033[1;33m_ \033[1;32m __ ___ \033[1;36m| |  __\033[1;34m| (___  \033[0m\n");
    printf(" \033[1;31m  \\ \\/ / \033[1;33m| |\033[1;32m/ _ ` _ \\\033[1;36m| | |_ \\\033[1;34m\\___ \\ \033[0m\n");
    printf(" \033[1;31m   \\  /  \033[1;33m| |\033[1;32m  __/ |_)\033[1;36m| |__| |\033[1;34m____) |\033[0m\n");
    printf(" \033[1;31m    \\/   \033[1;33m|_|\033[1;32m\\___|_.__/\033[1;36m\\_____|\033[1;34m_____/ \033[0m\n");
    printf("       \033[1;31mV\033[1;33mi\033[1;32mb\033[1;32me\033[1;36mO\033[1;34mS \033[1;37m- A Simple 32-bit Operating System\033[0m\n");
    printf("\n");
}

/* The kernel main function */
void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_addr) {
    /* First thing: initialize terminal and serial for output */
    terminal_init();
    serial_init();
    
    /* Display the VibeOS logo */
    print_logo();
    
    /* Check multiboot magic but don't fail if it's wrong */
    if (!CHECK_MULTIBOOT_MAGIC(multiboot_magic)) {
        printf("\033[33mWarning: Not booted with multiboot (QEMU direct boot mode)\033[0m\n");
    } else {
        printf("\033[32mMultiboot-compliant boot confirmed\033[0m\n");
    }
    
    /* Basic output to both console and serial */
    printf("\033[32mKernel booted successfully\033[0m\n\n");
    
    /* Initialize the IDT */
    idt_init();
    
    /* Register a custom handler for the divide by zero exception */
    register_interrupt_handler(0, test_interrupt_handler);
    
    /* Initialize the PS/2 keyboard */
    keyboard_init();
    
    /* Enable interrupts so keyboard can generate events */
    asm volatile ("sti");
    
    printf("\n\033[1;36mKeyboard ready! Start typing...\033[0m\n");
    
    /* Main kernel loop - halt when idle but wake on interrupts */
    while (1) {
        asm volatile ("hlt");
    }
} 