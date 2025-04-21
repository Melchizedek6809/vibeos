#include "serial.h"
#include "../include/stdint.h"
#include "../include/stddef.h"
#include "../include/stdarg.h"
#include "../arch/x86/io.h"
#include "include/stdio.h"

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

/* Printf style function for serial output */
void serial_printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    serial_write_string(buffer);
} 