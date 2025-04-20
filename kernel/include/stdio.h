#ifndef _VIBEOS_STDIO_H
#define _VIBEOS_STDIO_H

#include "stddef.h"
#include "stdint.h"
#include "stdarg.h"

/* Write a string to both terminal and serial outputs */
void puts(const char* str);

/* Format a string and output to both terminal and serial
 * This is the preferred output function that writes to both
 * VGA text mode display and serial console simultaneously.
 * 
 * Supported format specifiers:
 * %s - String
 * %c - Character
 * %d, %i - Signed integer
 * %u - Unsigned integer
 * %x - Hexadecimal
 * %p - Pointer (displayed as hex with 0x prefix)
 * %% - Literal %
 */
int printf(const char* format, ...);

/* Format a string and store it in the buffer */
int snprintf(char* buffer, size_t size, const char* format, ...);

/* Format a string with va_list args and store it in the buffer */
int vsnprintf(char* buffer, size_t size, const char* format, va_list args);

#endif /* _VIBEOS_STDIO_H */ 