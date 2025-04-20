#ifndef SERIAL_H
#define SERIAL_H

#include "include/stdint.h"

/* Initialize the serial port */
void serial_init(void);

/* Write a character to the serial port */
void serial_write_char(char c);

/* Write a string to the serial port */
void serial_write_string(const char* str);

#endif /* SERIAL_H */ 