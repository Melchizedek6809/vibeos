#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>

/* I/O port functions */
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);

/* Terminal functions */
void terminal_init(void);
void terminal_write(const char* data);

/* Serial port functions */
void serial_init(void);
void serial_write_char(char c);
void serial_write_string(const char* str);

/* Utility functions */
void delay(int count);

/* Kernel entry point - called from boot.S */
void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_addr);

#endif /* KERNEL_H */ 