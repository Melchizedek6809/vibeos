#include <stdint.h>
#include <stddef.h>
#include "kernel.h"

/* Multiboot header magic values */
#define MULTIBOOT_MAGIC 0x1BADB002
#define CHECK_MULTIBOOT_MAGIC(x) ((x) == MULTIBOOT_MAGIC)

/* Basic VGA text mode support */
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

/* Hardware text mode color constants */
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

static uint16_t* const VGA_BUFFER = (uint16_t*) VGA_MEMORY;
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;

/* Creates a VGA entry with character and color */
static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t) c | (uint16_t) color << 8;
}

/* Creates a color attribute from foreground and background */
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

/* Initializes the terminal (clears screen) */
void terminal_init(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    
    /* Clear the screen */
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            VGA_BUFFER[index] = vga_entry(' ', terminal_color);
        }
    }
}

/* Puts a character at a specific position */
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    VGA_BUFFER[index] = vga_entry(c, color);
}

/* Puts a character at the current position and advances cursor */
void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
        return;
    }
    
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    
    if (++terminal_column == VGA_WIDTH) {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
            terminal_row = 0;
    }
}

/* Writes a string to the terminal */
void terminal_write(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++)
        terminal_putchar(data[i]);
}

/* Serial port I/O functions */
#define COM1 0x3F8   /* COM1 port for serial output */

/* Check if transmit is empty */
static inline int serial_transmit_empty() {
    return inb(COM1 + 5) & 0x20;
}

/* Initialize serial port */
void serial_init() {
    outb(COM1 + 1, 0x00);    /* Disable all interrupts */
    outb(COM1 + 3, 0x80);    /* Enable DLAB (set baud rate divisor) */
    outb(COM1 + 0, 0x03);    /* Set divisor to 3 (lo byte) 38400 baud */
    outb(COM1 + 1, 0x00);    /* (hi byte) */
    outb(COM1 + 3, 0x03);    /* 8 bits, no parity, one stop bit */
    outb(COM1 + 2, 0xC7);    /* Enable FIFO, clear them, 14-byte threshold */
    outb(COM1 + 4, 0x0B);    /* IRQs enabled, RTS/DSR set */
}

/* Write a character to serial port */
void serial_write_char(char c) {
    /* Wait until we can send */
    while (serial_transmit_empty() == 0);
    
    /* Send the character */
    outb(COM1, c);
    
    /* If newline, also send carriage return */
    if (c == '\n') {
        while (serial_transmit_empty() == 0);
        outb(COM1, '\r');
    }
}

/* Write a string to serial port */
void serial_write_string(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++)
        serial_write_char(str[i]);
}

/* I/O port functions */
uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

/* Simple delay function - spins CPU */
void delay(int count) {
    for (int i = 0; i < count * 10000; i++)
        asm volatile ("nop");
}

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