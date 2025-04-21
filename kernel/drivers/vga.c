#include "vga.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../arch/x86/io.h"

/* Hardware text mode constants */
#define VGA_MEMORY 0xB8000
#define VGA_CTRL_REGISTER 0x3D4
#define VGA_DATA_REGISTER 0x3D5
#define VGA_CURSOR_HIGH 14
#define VGA_CURSOR_LOW 15

/* ANSI escape sequence states */
#define ANSI_STATE_NORMAL     0
#define ANSI_STATE_ESC        1
#define ANSI_STATE_BRACKET    2
#define ANSI_STATE_PARAMS     3

/* Terminal state */
static uint16_t* const VGA_BUFFER = (uint16_t*) VGA_MEMORY;
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint8_t terminal_default_color;
static int cursor_enabled = 1;

/* ANSI escape sequence parsing */
static int ansi_state = ANSI_STATE_NORMAL;
static char ansi_params[16];
static size_t ansi_param_index = 0;

/* Map ANSI color codes (0-7) to VGA colors */
static const enum vga_color ansi_to_vga_color[8] = {
    VGA_COLOR_BLACK,         /* ANSI: Black */
    VGA_COLOR_RED,           /* ANSI: Red */
    VGA_COLOR_GREEN,        /* ANSI: Green */
    VGA_COLOR_BROWN,        /* ANSI: Yellow */
    VGA_COLOR_BLUE,         /* ANSI: Blue */
    VGA_COLOR_MAGENTA,      /* ANSI: Magenta */
    VGA_COLOR_CYAN,         /* ANSI: Cyan */
    VGA_COLOR_LIGHT_GREY    /* ANSI: White */
};

/* Map ANSI bright color codes (8-15) to VGA colors */
static const enum vga_color ansi_to_vga_bright_color[8] = {
    VGA_COLOR_DARK_GREY,    /* ANSI: Bright Black (Gray) */
    VGA_COLOR_LIGHT_RED,    /* ANSI: Bright Red */
    VGA_COLOR_LIGHT_GREEN,  /* ANSI: Bright Green */
    VGA_COLOR_LIGHT_BROWN,  /* ANSI: Bright Yellow */
    VGA_COLOR_LIGHT_BLUE,   /* ANSI: Bright Blue */
    VGA_COLOR_LIGHT_MAGENTA,/* ANSI: Bright Magenta */
    VGA_COLOR_LIGHT_CYAN,   /* ANSI: Bright Cyan */
    VGA_COLOR_WHITE         /* ANSI: Bright White */
};

/* Update hardware cursor position */
static void update_cursor(void) {
    if (!cursor_enabled) return;
    
    size_t pos = terminal_row * VGA_WIDTH + terminal_column;
    
    outb(VGA_CTRL_REGISTER, VGA_CURSOR_HIGH);
    outb(VGA_DATA_REGISTER, (pos >> 8) & 0xFF);
    outb(VGA_CTRL_REGISTER, VGA_CURSOR_LOW);
    outb(VGA_DATA_REGISTER, pos & 0xFF);
}

/* Enable or disable the hardware cursor */
void terminal_cursor_enable(int enable) {
    cursor_enabled = enable;
    if (!enable) {
        outb(VGA_CTRL_REGISTER, 0x0A);
        outb(VGA_DATA_REGISTER, 0x20);
    }
}

/* Creates a VGA entry with character and color */
uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t) c | (uint16_t) color << 8;
}

/* Creates a color attribute from foreground and background */
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

/* Set terminal position */
vga_status_t terminal_setpos(size_t x, size_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
        return VGA_ERROR_INVALID_POSITION;
    }
    
    terminal_column = x;
    terminal_row = y;
    update_cursor();
    
    return VGA_SUCCESS;
}

/* Get terminal position */
vga_pos_t terminal_getpos(void) {
    return (vga_pos_t){terminal_column, terminal_row};
}

/* Set terminal colors */
vga_status_t terminal_setcolor(enum vga_color fg, enum vga_color bg) {
    if (fg > VGA_COLOR_WHITE || bg > VGA_COLOR_WHITE) {
        return VGA_ERROR_INVALID_COLOR;
    }
    
    terminal_color = vga_entry_color(fg, bg);
    return VGA_SUCCESS;
}

/* Get terminal color */
uint8_t terminal_getcolor(void) {
    return terminal_color;
}

/* Clear the terminal screen */
void terminal_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            VGA_BUFFER[index] = vga_entry(' ', terminal_color);
        }
    }
}

/* Scroll the terminal up by n lines */
vga_status_t terminal_scroll(size_t lines) {
    if (lines == 0) return VGA_SUCCESS;
    if (lines >= VGA_HEIGHT) {
        terminal_clear();
        return VGA_SUCCESS;
    }
    
    /* Move lines up */
    size_t move_size = (VGA_HEIGHT - lines) * VGA_WIDTH * sizeof(uint16_t);
    memmove(VGA_BUFFER, 
            VGA_BUFFER + (lines * VGA_WIDTH),
            move_size);
    
    /* Clear bottom lines */
    for (size_t y = VGA_HEIGHT - lines; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            VGA_BUFFER[index] = vga_entry(' ', terminal_color);
        }
    }
    
    return VGA_SUCCESS;
}

/* Initialize the terminal */
void terminal_init(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_default_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    terminal_color = terminal_default_color;
    
    terminal_clear();
    terminal_cursor_enable(1);
    
    /* Reset ANSI state */
    ansi_state = ANSI_STATE_NORMAL;
}

/* Put a character at a specific position */
vga_status_t terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
        return VGA_ERROR_INVALID_POSITION;
    }
    
    const size_t index = y * VGA_WIDTH + x;
    VGA_BUFFER[index] = vga_entry(c, color);
    
    return VGA_SUCCESS;
}

/* Process ANSI SGR (Select Graphic Rendition) parameters */
static void process_ansi_params(void) {
    /* Add a null terminator */
    ansi_params[ansi_param_index] = '\0';
    
    /* Split the parameter string by semicolons */
    char* param = ansi_params;
    char* next_param;
    
    while (param && *param) {
        /* Find the next parameter (after semicolon) */
        next_param = strchr(param, ';');
        if (next_param) {
            *next_param = '\0';
            next_param++;
        }
        
        /* Convert the parameter to an integer */
        int code = atoi(param);
        
        /* Process the SGR code */
        switch (code) {
            case 0: /* Reset / Normal */
                terminal_color = terminal_default_color;
                break;
                
            case 1: /* Bold/Bright (increase color intensity) */
                /* If foreground is standard color (0-7), make it bright (8-15) */
                if ((terminal_color & 0x0F) < 8) {
                    terminal_color = (terminal_color & 0xF0) | 
                                    ((terminal_color & 0x0F) + 8);
                }
                break;
                
            case 30: case 31: case 32: case 33: /* Foreground colors */
            case 34: case 35: case 36: case 37:
                /* Set foreground color (code - 30 is the color index) */
                terminal_color = (terminal_color & 0xF0) | 
                                ansi_to_vga_color[code - 30];
                break;
                
            case 40: case 41: case 42: case 43: /* Background colors */
            case 44: case 45: case 46: case 47:
                /* Set background color (code - 40 is the color index) */
                terminal_color = (terminal_color & 0x0F) | 
                                (ansi_to_vga_color[code - 40] << 4);
                break;
                
            case 90: case 91: case 92: case 93: /* Bright foreground colors */
            case 94: case 95: case 96: case 97:
                /* Set bright foreground color (code - 90 is the color index) */
                terminal_color = (terminal_color & 0xF0) | 
                                ansi_to_vga_bright_color[code - 90];
                break;
                
            case 100: case 101: case 102: case 103: /* Bright background colors */
            case 104: case 105: case 106: case 107:
                /* Set bright background color (code - 100 is the color index) */
                terminal_color = (terminal_color & 0x0F) | 
                                (ansi_to_vga_bright_color[code - 100] << 4);
                break;
        }
        
        /* Move to the next parameter */
        param = next_param;
    }
}

/* Put a character at the current position */
vga_status_t terminal_putchar(char c) {
    vga_status_t status = VGA_SUCCESS;
    
    /* ANSI escape sequence processing */
    if (ansi_state == ANSI_STATE_NORMAL) {
        if (c == '\033') { /* ESC character */
            ansi_state = ANSI_STATE_ESC;
            return VGA_SUCCESS;
        }
    } else if (ansi_state == ANSI_STATE_ESC) {
        if (c == '[') {
            ansi_state = ANSI_STATE_BRACKET;
            ansi_param_index = 0;
            memset(ansi_params, 0, sizeof(ansi_params));
            return VGA_SUCCESS;
        } else {
            ansi_state = ANSI_STATE_NORMAL;
        }
    } else if (ansi_state == ANSI_STATE_BRACKET) {
        if (c >= '0' && c <= '9') {
            ansi_state = ANSI_STATE_PARAMS;
            ansi_params[ansi_param_index++] = c;
            return VGA_SUCCESS;
        } else {
            ansi_state = ANSI_STATE_NORMAL;
        }
    } else if (ansi_state == ANSI_STATE_PARAMS) {
        if ((c >= '0' && c <= '9') || c == ';') {
            if (ansi_param_index < sizeof(ansi_params) - 1) {
                ansi_params[ansi_param_index++] = c;
            }
            return VGA_SUCCESS;
        } else if (c == 'm') {
            process_ansi_params();
            ansi_state = ANSI_STATE_NORMAL;
            return VGA_SUCCESS;
        } else {
            ansi_state = ANSI_STATE_NORMAL;
        }
    }
    
    /* Handle special characters */
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            status = terminal_scroll(1);
            terminal_row = VGA_HEIGHT - 1;
        }
    } else if (c == '\r') {
        terminal_column = 0;
    } else if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
            status = terminal_putentryat(' ', terminal_color, 
                                       terminal_column, terminal_row);
        }
    } else if (c == '\t') {
        /* Tab stops every 8 columns */
        size_t spaces = 8 - (terminal_column % 8);
        for (size_t i = 0; i < spaces && terminal_column < VGA_WIDTH; i++) {
            status = terminal_putentryat(' ', terminal_color, 
                                       terminal_column++, terminal_row);
            if (status != VGA_SUCCESS) break;
        }
    } else {
        status = terminal_putentryat(c, terminal_color, 
                                   terminal_column, terminal_row);
        if (status == VGA_SUCCESS && ++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                status = terminal_scroll(1);
                terminal_row = VGA_HEIGHT - 1;
            }
        }
    }
    
    update_cursor();
    return status;
}

/* Write a string to the terminal */
vga_status_t terminal_write(const char* data) {
    vga_status_t status = VGA_SUCCESS;
    
    for (size_t i = 0; data[i] != '\0'; i++) {
        status = terminal_putchar(data[i]);
        if (status != VGA_SUCCESS) break;
    }
    
    return status;
}

/* Write a string with specific color */
vga_status_t terminal_write_color(const char* data, enum vga_color fg, enum vga_color bg) {
    uint8_t old_color = terminal_color;
    vga_status_t status = terminal_setcolor(fg, bg);
    
    if (status == VGA_SUCCESS) {
        status = terminal_write(data);
        terminal_color = old_color;
    }
    
    return status;
}

/* Printf style function for terminal output */
int vga_printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    
    va_start(args, format);
    int result = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (result < 0) {
        return result;
    }
    
    vga_status_t status = terminal_write(buffer);
    return (status == VGA_SUCCESS) ? result : (int)status;
} 