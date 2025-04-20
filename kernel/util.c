#include "util.h"
#include "vga.h"
#include "serial.h"

/* Simple delay function - spins CPU */
void delay(int count) {
    for (int i = 0; i < count * 10000; i++)
        asm volatile ("nop");
}

/* Write a string to both terminal and serial */
void puts(const char* str) {
    terminal_write(str);
    serial_write_string(str);
} 