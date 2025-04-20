#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>

/* Text mode color constants */
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

/* Create a VGA color from foreground and background colors */
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);

/* Create a VGA entry from a character and color */
uint16_t vga_entry(unsigned char c, uint8_t color);

/* Initialize the terminal (clear screen) */
void terminal_init(void);

/* Put a character at a specific position */
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);

/* Put a character at the current position */
void terminal_putchar(char c);

/* Write a string to the terminal */
void terminal_write(const char* data);

#endif /* VGA_H */ 