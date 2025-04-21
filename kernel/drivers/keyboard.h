#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "../arch/x86/idt.h"

/* PS/2 keyboard ports */
#define KEYBOARD_DATA_PORT      0x60
#define KEYBOARD_STATUS_PORT    0x64
#define KEYBOARD_COMMAND_PORT   0x64

/* PS/2 Controller Commands */
#define KEYBOARD_CMD_READ_CONFIG    0x20
#define KEYBOARD_CMD_WRITE_CONFIG   0x60
#define KEYBOARD_CMD_DISABLE_PORT2  0xA7
#define KEYBOARD_CMD_ENABLE_PORT2   0xA8
#define KEYBOARD_CMD_TEST_PORT2     0xA9
#define KEYBOARD_CMD_TEST_PS2       0xAA
#define KEYBOARD_CMD_DISABLE_PORT1  0xAD
#define KEYBOARD_CMD_ENABLE_PORT1   0xAE
#define KEYBOARD_CMD_TEST_PORT1     0xAB

/* Keyboard status codes */
typedef enum {
    KEYBOARD_SUCCESS = 0,
    KEYBOARD_ERROR_TIMEOUT = -1,
    KEYBOARD_ERROR_INIT_FAILED = -2,
    KEYBOARD_ERROR_SELF_TEST_FAILED = -3,
    KEYBOARD_NO_DATA = -4
} keyboard_status_t;

/* Key states */
typedef enum {
    KEY_STATE_UP = 0,
    KEY_STATE_DOWN = 1,
    KEY_STATE_REPEAT = 2
} key_state_t;

/* Modifier key states */
typedef struct {
    uint8_t shift : 1;
    uint8_t ctrl : 1;
    uint8_t alt : 1;
    uint8_t capslock : 1;
    uint8_t numlock : 1;
    uint8_t scrolllock : 1;
    uint8_t reserved : 2;
} keyboard_modifiers_t;

/* Scancode set 1 (US QWERTY layout) */
typedef enum {
    KEY_ERROR = 0,
    KEY_ESC = 27,
    KEY_BACKSPACE = '\b',
    KEY_TAB = '\t',
    KEY_ENTER = '\n',
    KEY_CTRL = 0x1D,
    KEY_LSHIFT = 0x2A,
    KEY_RSHIFT = 0x36,
    KEY_ALT = 0x38,
    KEY_SPACE = ' ',
    KEY_CAPSLOCK = 0x3A,
    KEY_F1 = 0x3B,
    KEY_F2 = 0x3C,
    KEY_F3 = 0x3D,
    KEY_F4 = 0x3E,
    KEY_F5 = 0x3F,
    KEY_F6 = 0x40,
    KEY_F7 = 0x41,
    KEY_F8 = 0x42,
    KEY_F9 = 0x43,
    KEY_F10 = 0x44,
    KEY_F11 = 0x57,
    KEY_F12 = 0x58,
    KEY_NUMLOCK = 0x45,
    KEY_SCROLLLOCK = 0x46
} special_keys_t;

/* Initialize the PS/2 keyboard
 * Returns: KEYBOARD_SUCCESS on success, error code otherwise
 */
keyboard_status_t keyboard_init(void);

/* Wait for keyboard input buffer to be ready to read
 * Returns: KEYBOARD_SUCCESS on success, KEYBOARD_ERROR_TIMEOUT on timeout
 */
keyboard_status_t keyboard_wait_read(void);

/* Wait for keyboard input buffer to be ready to write
 * Returns: KEYBOARD_SUCCESS on success, KEYBOARD_ERROR_TIMEOUT on timeout
 */
keyboard_status_t keyboard_wait_write(void);

/* Process keyboard input and return the ASCII character, if any
 * Returns: ASCII character if available, 0 otherwise
 */
char keyboard_process_input(void);

/* Convert a scancode to an ASCII character
 * Returns: ASCII character if valid scancode, 0 otherwise
 */
char keyboard_scancode_to_ascii(uint8_t scancode);

/* Get current state of modifier keys
 * Returns: Current modifier key states
 */
keyboard_modifiers_t keyboard_get_modifiers(void);

/* Check if a specific key is currently pressed
 * Returns: 1 if key is pressed, 0 otherwise
 */
int keyboard_is_key_pressed(uint8_t scancode);

/* Keyboard interrupt handler */
void keyboard_handler(registers_t* regs);

#endif /* KEYBOARD_H */ 