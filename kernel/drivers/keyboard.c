#include "keyboard.h"
#include <stdint.h>
#include <stdio.h>
#include "../arch/x86/io.h"
#include "../include/idt.h"

/* PS/2 keyboard IRQ number */
#define KEYBOARD_IRQ 1

/* Flag to track shift key state */
static int shift_pressed = 0;
static int capslock_on = 0;

/* Scancode set 1 (US QWERTY layout) lookup table 
 * Index is the scancode, value is the ASCII character 
 */
static const char scancode_to_ascii_low[128] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, 
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Uppercase and special character lookup table */
static const char scancode_to_ascii_high[128] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, 
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Initialize the PS/2 keyboard */
void keyboard_init(void) {
    /* Register keyboard IRQ handler */
    register_interrupt_handler(KEYBOARD_IRQ + 32, keyboard_handler);
    
    /* Simple initialization - flush the output buffer */
    if (inb(KEYBOARD_STATUS_PORT) & 0x01) {
        inb(KEYBOARD_DATA_PORT);
    }
    
    printf("\033[1;32mKeyboard initialized\033[0m\n");
}

/* Wait for the keyboard controller to be ready to accept input */
void keyboard_wait_write(void) {
    /* Wait until the status bit for input buffer full is clear */
    while ((inb(KEYBOARD_STATUS_PORT) & 0x2) != 0) {
        /* Small delay */
        for (int i = 0; i < 100; i++) { }
    }
}

/* Wait for the keyboard controller to be ready to send data */
void keyboard_wait_read(void) {
    /* Wait until the status bit for output buffer full is set */
    while ((inb(KEYBOARD_STATUS_PORT) & 0x1) == 0) {
        /* Small delay */
        for (int i = 0; i < 100; i++) { }
    }
}

/* Keyboard interrupt handler */
void keyboard_handler(registers_t* regs) {
    (void)regs; /* Avoid unused parameter warning */
    
    /* Read the scancode from the keyboard data port */
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    /* Process the scancode */
    if (scancode < 128) {
        /* Key press */
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = 1;
        } else if (scancode == KEY_CAPSLOCK) {
            capslock_on = !capslock_on;
        } else {
            /* Convert scancode to ASCII and print the character if it's valid */
            char ascii = keyboard_scancode_to_ascii(scancode);
            if (ascii) {
                printf("%c", ascii);
            }
        }
    } else {
        /* Key release (scancode >= 128) */
        uint8_t released_key = scancode & 0x7F; /* Remove the release bit */
        
        if (released_key == KEY_LSHIFT || released_key == KEY_RSHIFT) {
            shift_pressed = 0;
        }
    }
}

/* Convert a scancode to an ASCII character */
char keyboard_scancode_to_ascii(uint8_t scancode) {
    /* Check if the scancode is within bounds */
    if (scancode >= 128) {
        return 0; /* Key release events don't generate ASCII */
    }
    
    /* Determine if we should use uppercase table */
    int uppercase = 0;
    
    /* Letters are capitalized if (shift XOR capslock) is true */
    if (scancode >= 16 && scancode <= 25) { /* Q-P */
        uppercase = shift_pressed ^ capslock_on;
    } else if (scancode >= 30 && scancode <= 38) { /* A-L */
        uppercase = shift_pressed ^ capslock_on;
    } else if (scancode >= 44 && scancode <= 50) { /* Z-M */
        uppercase = shift_pressed ^ capslock_on;
    } else {
        /* For non-letter characters, we use upper symbols only with shift */
        uppercase = shift_pressed;
    }
    
    /* Return the appropriate character based on the current modifier state */
    return uppercase ? scancode_to_ascii_high[scancode] : scancode_to_ascii_low[scancode];
}

/* Process keyboard input and return the ASCII character, if any */
char keyboard_process_input(void) {
    /* Check if there's data in the keyboard buffer */
    if ((inb(KEYBOARD_STATUS_PORT) & 0x1) == 0) {
        return 0; /* No data available */
    }
    
    /* Read the scancode */
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    /* Process the scancode */
    if (scancode < 128) {
        /* Key press */
        if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
            shift_pressed = 1;
            return 0;
        } else if (scancode == KEY_CAPSLOCK) {
            capslock_on = !capslock_on;
            return 0;
        } else {
            /* Convert scancode to ASCII */
            return keyboard_scancode_to_ascii(scancode);
        }
    } else {
        /* Key release (scancode >= 128) */
        uint8_t released_key = scancode & 0x7F; /* Remove the release bit */
        
        if (released_key == KEY_LSHIFT || released_key == KEY_RSHIFT) {
            shift_pressed = 0;
        }
        
        return 0; /* Key releases don't generate ASCII */
    }
} 