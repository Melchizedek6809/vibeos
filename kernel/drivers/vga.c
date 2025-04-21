#include "vga.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "../arch/x86/io.h"

/* Hardware text mode color constants */
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

/* ANSI escape sequence states */
#define ANSI_STATE_NORMAL     0
#define ANSI_STATE_ESC        1
#define ANSI_STATE_BRACKET    2
#define ANSI_STATE_PARAMS     3

static uint16_t* const VGA_BUFFER = (uint16_t*) VGA_MEMORY;
static size_t terminal_row;
static size_t terminal_column;
static uint8_t terminal_color;
static uint8_t terminal_default_color;

/* ANSI escape sequence parsing */
static int ansi_state = ANSI_STATE_NORMAL;
static char ansi_params[16];
static size_t ansi_param_index = 0;

/* Map ANSI color codes (0-7) to VGA colors */
static const enum vga_color ansi_to_vga_color[8] = {
    VGA_COLOR_BLACK,         /* ANSI: Black */
    VGA_COLOR_RED,           /* ANSI: Red */
    VGA_COLOR_GREEN,         /* ANSI: Green */
    VGA_COLOR_BROWN,         /* ANSI: Yellow */
    VGA_COLOR_BLUE,          /* ANSI: Blue */
    VGA_COLOR_MAGENTA,       /* ANSI: Magenta */
    VGA_COLOR_CYAN,          /* ANSI: Cyan */
    VGA_COLOR_LIGHT_GREY     /* ANSI: White */
};

/* Map ANSI bright color codes (8-15) to VGA colors */
static const enum vga_color ansi_to_vga_bright_color[8] = {
    VGA_COLOR_DARK_GREY,     /* ANSI: Bright Black (Gray) */
    VGA_COLOR_LIGHT_RED,     /* ANSI: Bright Red */
    VGA_COLOR_LIGHT_GREEN,   /* ANSI: Bright Green */
    VGA_COLOR_LIGHT_BROWN,   /* ANSI: Bright Yellow */
    VGA_COLOR_LIGHT_BLUE,    /* ANSI: Bright Blue */
    VGA_COLOR_LIGHT_MAGENTA, /* ANSI: Bright Magenta */
    VGA_COLOR_LIGHT_CYAN,    /* ANSI: Bright Cyan */
    VGA_COLOR_WHITE          /* ANSI: Bright White */
};

/* Creates a VGA entry with character and color */
uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t) c | (uint16_t) color << 8;
}

/* Creates a color attribute from foreground and background */
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

/* Initializes the terminal (clears screen) */
void terminal_init(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_default_color = vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    terminal_color = terminal_default_color;
    
    /* Clear the screen */
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            VGA_BUFFER[index] = vga_entry(' ', terminal_color);
        }
    }
    
    /* Reset ANSI state */
    ansi_state = ANSI_STATE_NORMAL;
}

/* Puts a character at a specific position */
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
    const size_t index = y * VGA_WIDTH + x;
    VGA_BUFFER[index] = vga_entry(c, color);
}

/* Process ANSI SGR (Select Graphic Rendition) parameters */
static void process_ansi_params() {
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

/* Puts a character at the current position and advances cursor */
void terminal_putchar(char c) {
    /* ANSI escape sequence processing */
    if (ansi_state == ANSI_STATE_NORMAL) {
        if (c == '\033') { /* ESC character */
            ansi_state = ANSI_STATE_ESC;
            return;
        }
    } else if (ansi_state == ANSI_STATE_ESC) {
        if (c == '[') {
            ansi_state = ANSI_STATE_BRACKET;
            ansi_param_index = 0;
            memset(ansi_params, 0, sizeof(ansi_params));
            return;
        } else {
            /* Not a supported escape sequence, revert to normal state */
            ansi_state = ANSI_STATE_NORMAL;
        }
    } else if (ansi_state == ANSI_STATE_BRACKET) {
        if (c >= '0' && c <= '9') {
            /* Start of parameters */
            ansi_state = ANSI_STATE_PARAMS;
            ansi_params[ansi_param_index++] = c;
            return;
        } else {
            /* Not a supported escape sequence, revert to normal state */
            ansi_state = ANSI_STATE_NORMAL;
        }
    } else if (ansi_state == ANSI_STATE_PARAMS) {
        if ((c >= '0' && c <= '9') || c == ';') {
            /* Continue collecting parameters */
            if (ansi_param_index < sizeof(ansi_params) - 1) {
                ansi_params[ansi_param_index++] = c;
            }
            return;
        } else if (c == 'm') {
            /* SGR (Select Graphic Rendition) command */
            process_ansi_params();
            ansi_state = ANSI_STATE_NORMAL;
            return;
        } else {
            /* Not a supported command, revert to normal state */
            ansi_state = ANSI_STATE_NORMAL;
        }
    }
    
    /* Normal character processing */
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

/* Printf style function for terminal output */
void vga_printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    terminal_write(buffer);
} 