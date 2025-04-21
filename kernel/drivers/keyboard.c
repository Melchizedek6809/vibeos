#include "keyboard.h"
#include <stdint.h>
#include <stdio.h>
#include "../arch/x86/io.h"
#include "../arch/x86/idt.h"

/* PS/2 keyboard IRQ number */
#define KEYBOARD_IRQ 1

/* Keyboard state tracking */
static keyboard_modifiers_t modifiers = {0};
static uint8_t key_states[128] = {0};  /* Track state of each key */

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
keyboard_status_t keyboard_init(void) {
    /* Simple initialization - just flush the output buffer */
    while (inb(KEYBOARD_STATUS_PORT) & 0x1) {
        inb(KEYBOARD_DATA_PORT);
    }
    
    /* Clear all modifier keys and states */
    modifiers = (keyboard_modifiers_t){0};
    for (int i = 0; i < 128; i++) {
        key_states[i] = 0;
    }
    
    /* Register keyboard IRQ handler */
    register_interrupt_handler(KEYBOARD_IRQ + 32, keyboard_handler);
    
    printf("\033[1;32mKeyboard initialized\033[0m\n");
    return KEYBOARD_SUCCESS;
}

/* Wait for the keyboard controller to be ready to accept input */
keyboard_status_t keyboard_wait_write(void) {
    int timeout = 1000;
    while (timeout-- && (inb(KEYBOARD_STATUS_PORT) & 0x2)) {
        /* Small delay */
        for (int i = 0; i < 100; i++) { }
    }
    return timeout ? KEYBOARD_SUCCESS : KEYBOARD_ERROR_TIMEOUT;
}

/* Wait for the keyboard controller to be ready to send data */
keyboard_status_t keyboard_wait_read(void) {
    int timeout = 1000;
    while (timeout-- && !(inb(KEYBOARD_STATUS_PORT) & 0x1)) {
        /* Small delay */
        for (int i = 0; i < 100; i++) { }
    }
    return timeout ? KEYBOARD_SUCCESS : KEYBOARD_ERROR_TIMEOUT;
}

/* Get current state of modifier keys */
keyboard_modifiers_t keyboard_get_modifiers(void) {
    return modifiers;
}

/* Check if a specific key is currently pressed */
int keyboard_is_key_pressed(uint8_t scancode) {
    if (scancode >= 128) return 0;
    return key_states[scancode] == KEY_STATE_DOWN;
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
    if ((scancode >= 16 && scancode <= 25) ||   /* Q-P */
        (scancode >= 30 && scancode <= 38) ||   /* A-L */
        (scancode >= 44 && scancode <= 50)) {   /* Z-M */
        uppercase = modifiers.shift ^ modifiers.capslock;
    } else {
        /* For non-letter characters, we use upper symbols only with shift */
        uppercase = modifiers.shift;
    }
    
    /* Return the appropriate character based on the current modifier state */
    return uppercase ? scancode_to_ascii_high[scancode] : scancode_to_ascii_low[scancode];
}

/* Keyboard interrupt handler */
void keyboard_handler(registers_t* regs) {
    (void)regs; /* Avoid unused parameter warning */
    
    /* Read the scancode */
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    /* Update key state */
    if (scancode < 128) {
        /* Key press */
        key_states[scancode] = KEY_STATE_DOWN;
        
        /* Update modifier states */
        switch (scancode) {
            case KEY_LSHIFT:
            case KEY_RSHIFT:
                modifiers.shift = 1;
                break;
            case KEY_CTRL:
                modifiers.ctrl = 1;
                break;
            case KEY_ALT:
                modifiers.alt = 1;
                break;
            case KEY_CAPSLOCK:
                modifiers.capslock = !modifiers.capslock;
                break;
            case KEY_NUMLOCK:
                modifiers.numlock = !modifiers.numlock;
                break;
            case KEY_SCROLLLOCK:
                modifiers.scrolllock = !modifiers.scrolllock;
                break;
            default:
                /* Convert scancode to ASCII and print if it's a printable character */
                char ascii = keyboard_scancode_to_ascii(scancode);
                if (ascii) {
                    printf("%c", ascii);
                }
                break;
        }
    } else {
        /* Key release */
        uint8_t released_key = scancode & 0x7F;
        key_states[released_key] = KEY_STATE_UP;
        
        /* Update modifier states */
        switch (released_key) {
            case KEY_LSHIFT:
            case KEY_RSHIFT:
                modifiers.shift = 0;
                break;
            case KEY_CTRL:
                modifiers.ctrl = 0;
                break;
            case KEY_ALT:
                modifiers.alt = 0;
                break;
        }
    }
}

/* Process keyboard input and return the ASCII character, if any */
char keyboard_process_input(void) {
    /* Check if there's data in the keyboard buffer */
    if (!(inb(KEYBOARD_STATUS_PORT) & 0x1)) {
        return 0; /* No data available */
    }
    
    /* Read the scancode */
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    /* Update key state and return ASCII if available */
    if (scancode < 128) {
        key_states[scancode] = KEY_STATE_DOWN;
        return keyboard_scancode_to_ascii(scancode);
    } else {
        uint8_t released_key = scancode & 0x7F;
        key_states[released_key] = KEY_STATE_UP;
        return 0;
    }
} 