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
 * %x - Hexadecimal (lowercase)
 * %p - Pointer (displayed as hex with 0x prefix)
 * %% - Literal %
 * 
 * Width and padding modifiers:
 * %Ns - Right-pad with spaces to width N (e.g., %10s)
 * %0Nd - Left-pad with zeros to width N (e.g., %05d)
 * %Nd - Left-pad with spaces to width N (e.g., %5d)
 * 
 * Examples:
 * printf("%s", "hello")      -> "hello"
 * printf("%10s", "hello")    -> "     hello"
 * printf("%05d", 42)         -> "00042"
 * printf("%p", ptr)          -> "0x00001234"
 */
int printf(const char* format, ...);

/* Format a string and store it in the buffer
 * Returns the number of characters that would have been written if buffer had enough space
 * The output is always null-terminated if size > 0
 */
int snprintf(char* buffer, size_t size, const char* format, ...);

/* Format a string with va_list args and store it in the buffer
 * This is the core formatting function used by printf and snprintf
 * Returns the number of characters that would have been written if buffer had enough space
 * The output is always null-terminated if size > 0
 */
int vsnprintf(char* buffer, size_t size, const char* format, va_list args);

#endif /* _VIBEOS_STDIO_H */ 