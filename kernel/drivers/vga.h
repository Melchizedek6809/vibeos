#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* VGA dimensions */
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

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

/* Terminal status codes */
typedef enum {
    VGA_SUCCESS = 0,
    VGA_ERROR_INVALID_POSITION = -1,
    VGA_ERROR_INVALID_COLOR = -2
} vga_status_t;

/* Terminal cursor position */
typedef struct {
    size_t x;
    size_t y;
} vga_pos_t;

/* Create a VGA color from foreground and background colors
 * Returns: Combined color attribute
 */
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);

/* Create a VGA entry from a character and color
 * Returns: Combined character and attribute word
 */
uint16_t vga_entry(unsigned char c, uint8_t color);

/* Initialize the terminal
 * Clears the screen and sets default colors
 */
void terminal_init(void);

/* Set the terminal cursor position
 * Returns: VGA_SUCCESS on success, error code otherwise
 */
vga_status_t terminal_setpos(size_t x, size_t y);

/* Get the current terminal cursor position
 * Returns: Current cursor position
 */
vga_pos_t terminal_getpos(void);

/* Set the terminal colors
 * Returns: VGA_SUCCESS on success, error code otherwise
 */
vga_status_t terminal_setcolor(enum vga_color fg, enum vga_color bg);

/* Get the current terminal colors
 * Returns: Current color attribute
 */
uint8_t terminal_getcolor(void);

/* Clear the terminal screen
 * Fills the screen with spaces using current color
 */
void terminal_clear(void);

/* Scroll the terminal up by n lines
 * Returns: VGA_SUCCESS on success, error code otherwise
 */
vga_status_t terminal_scroll(size_t lines);

/* Put a character at a specific position with specific color
 * Returns: VGA_SUCCESS on success, error code otherwise
 */
vga_status_t terminal_putentryat(char c, uint8_t color, size_t x, size_t y);

/* Put a character at the current position
 * Handles special characters (newline, backspace, etc.)
 * Supports ANSI escape sequences for colors
 * Returns: VGA_SUCCESS on success, error code otherwise
 */
vga_status_t terminal_putchar(char c);

/* Write a string to the terminal
 * Returns: VGA_SUCCESS on success, error code otherwise
 */
vga_status_t terminal_write(const char* data);

/* Write a string with specific color to the terminal
 * Returns: VGA_SUCCESS on success, error code otherwise
 */
vga_status_t terminal_write_color(const char* data, enum vga_color fg, enum vga_color bg);

/* Printf style function for terminal output
 * Returns: Number of characters written on success, negative error code otherwise
 */
int vga_printf(const char* format, ...);

#endif /* VGA_H */ 