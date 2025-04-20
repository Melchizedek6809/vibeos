#include "../include/stdio.h"
#include "../include/stddef.h"
#include "../include/stdint.h"
#include "../include/stdarg.h"
#include "../include/string.h"
#include "../vga.h"
#include "../serial.h"

/* Write a string to both terminal and serial outputs */
void puts(const char* str) {
    terminal_write(str);
    serial_write_string(str);
}

/* Format a string and store it in the buffer */
int snprintf(char* buffer, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(buffer, size, format, args);
    va_end(args);
    return result;
}

/* Format a string with va_list args and store it in the buffer */
int vsnprintf(char* buffer, size_t size, const char* format, va_list args) {
    size_t count = 0;
    size_t i = 0;
    char temp_buf[64];  /* Temporary buffer for number conversions */
    
    /* Process format string */
    for (size_t pos = 0; format[pos] != '\0'; pos++) {
        /* Handle format specifiers */
        if (format[pos] == '%' && format[pos + 1] != '\0') {
            pos++;  /* Move to the format specifier */
            
            switch (format[pos]) {
                case 's': {  /* String */
                    const char* str = va_arg(args, const char*);
                    if (str == NULL) str = "(null)";
                    
                    while (*str && i < size - 1) {
                        buffer[i++] = *str++;
                        count++;
                    }
                    break;
                }
                
                case 'c': {  /* Character */
                    char c = (char)va_arg(args, int);
                    if (i < size - 1) {
                        buffer[i++] = c;
                        count++;
                    }
                    break;
                }
                
                case 'd': 
                case 'i': {  /* Signed integer */
                    int value = va_arg(args, int);
                    int len = itoa(value, temp_buf, 10);
                    
                    for (int j = 0; j < len && i < size - 1; j++) {
                        buffer[i++] = temp_buf[j];
                        count++;
                    }
                    break;
                }
                
                case 'u': {  /* Unsigned integer */
                    unsigned int value = va_arg(args, unsigned int);
                    int len = utoa(value, temp_buf, 10);
                    
                    for (int j = 0; j < len && i < size - 1; j++) {
                        buffer[i++] = temp_buf[j];
                        count++;
                    }
                    break;
                }
                
                case 'x': {  /* Hexadecimal (lowercase) */
                    unsigned int value = va_arg(args, unsigned int);
                    int len = utoa(value, temp_buf, 16);
                    
                    for (int j = 0; j < len && i < size - 1; j++) {
                        buffer[i++] = temp_buf[j];
                        count++;
                    }
                    break;
                }
                
                case 'p': {  /* Pointer */
                    void* value = va_arg(args, void*);
                    
                    /* Add 0x prefix */
                    if (i < size - 1) buffer[i++] = '0';
                    if (i < size - 1) buffer[i++] = 'x';
                    count += 2;
                    
                    int len = utoa((uint32_t)value, temp_buf, 16);
                    
                    for (int j = 0; j < len && i < size - 1; j++) {
                        buffer[i++] = temp_buf[j];
                        count++;
                    }
                    break;
                }
                
                case '%': {  /* Escaped % character */
                    if (i < size - 1) {
                        buffer[i++] = '%';
                        count++;
                    }
                    break;
                }
                
                default:  /* Unsupported format, just output as-is */
                    if (i < size - 1) {
                        buffer[i++] = '%';
                        count++;
                    }
                    if (i < size - 1) {
                        buffer[i++] = format[pos];
                        count++;
                    }
                    break;
            }
        } else {
            /* Regular character */
            if (i < size - 1) {
                buffer[i++] = format[pos];
                count++;
            }
        }
    }
    
    /* Ensure null-termination */
    if (size > 0) {
        buffer[i < size ? i : size - 1] = '\0';
    }
    
    return count;
} 