#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "../drivers/vga.h"
#include "../drivers/serial.h"

/* Write a string to both terminal and serial outputs */
void puts(const char* str) {
    terminal_write(str);
    serial_write_string(str);
}

/* Format a string and output to both terminal and serial */
int printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    
    va_start(args, format);
    int result = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    /* Output to both VGA and serial */
    terminal_write(buffer);
    serial_write_string(buffer);
    
    return result;
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
    
    /* Ensure size is at least 1 for null terminator */
    if (size == 0) return 0;
    size_t max_chars = size - 1; // Reserve space for null terminator

    /* Process format string */
    for (size_t pos = 0; format[pos] != '\0'; pos++) {
        if (i >= max_chars) break; // Stop if buffer is full

        /* Handle format specifiers */
        if (format[pos] == '%' && format[pos + 1] != '\0') {
            pos++;  /* Move past the '%' */
            
            // Parse flags and width
            char pad_char = ' '; // Default padding character is space
            int min_width = 0;
            int is_zero_padded = 0;

            // Check for zero-padding flag
            if (format[pos] == '0') {
                is_zero_padded = 1;
                pad_char = '0';
                pos++;
            }

            // Check for minimum width
            while (format[pos] >= '0' && format[pos] <= '9') {
                min_width = min_width * 10 + (format[pos] - '0');
                pos++;
            }
            
            // Handle the actual format specifier
            switch (format[pos]) {
                case 's': {  /* String */
                    const char* str = va_arg(args, const char*);
                    if (str == NULL) str = "(null)";
                    size_t str_len = strlen(str);
                    int padding = min_width > str_len ? min_width - str_len : 0;
                    
                    // Apply space padding (left-aligned by default, no zero padding for strings)
                    for (int p = 0; p < padding && i < max_chars; p++) {
                        buffer[i++] = ' ';
                        count++;
                    }
                    while (*str && i < max_chars) {
                        buffer[i++] = *str++;
                        count++;
                    }
                    break;
                }
                
                case 'c': {  /* Character */
                    char c = (char)va_arg(args, int);
                    int padding = min_width > 1 ? min_width - 1 : 0;
                    // Apply space padding
                    for (int p = 0; p < padding && i < max_chars; p++) {
                        buffer[i++] = ' ';
                        count++;
                    }
                    if (i < max_chars) {
                        buffer[i++] = c;
                        count++;
                    }
                    break;
                }
                
                case 'd': 
                case 'i': {  /* Signed integer */
                    int value = va_arg(args, int);
                    int len = itoa(value, temp_buf, 10);
                    int padding = min_width > len ? min_width - len : 0;
                    
                    // Handle sign for zero padding
                    if (value < 0 && is_zero_padded) {
                        if (i < max_chars) buffer[i++] = '-';
                        temp_buf[0] = '0'; // Remove sign from temp_buf for padding logic
                    }

                    // Apply padding
                    for (int p = 0; p < padding && i < max_chars; p++) {
                        buffer[i++] = pad_char;
                        count++;
                    }
                    // Copy number string
                    for (int j = (value < 0 && is_zero_padded) ? 1 : 0; j < len && i < max_chars; j++) {
                        buffer[i++] = temp_buf[j];
                        count++;
                    }
                    break;
                }
                
                case 'u':
                case 'x': {  /* Unsigned integer and Hexadecimal (lowercase) */
                    unsigned int value = va_arg(args, unsigned int);
                    int base = (format[pos] == 'u') ? 10 : 16;
                    int len = utoa(value, temp_buf, base);
                    int padding = min_width > len ? min_width - len : 0;
                    
                     // Handle 0x prefix for %p or future %#x (manual for now based on pci.c example)
                    int prefix_len = 0;
                    // if (format[pos] == 'p' || (/* other conditions for # flag */)) { ... }
                    
                    // Apply padding (pad_char determined by '0' flag)
                    for (int p = 0; p < padding && i < max_chars; p++) {
                        buffer[i++] = pad_char;
                        count++;
                    }
                     // Add prefix if needed (e.g. "0x" for pointers)
                    // if (prefix_len > 0 && i < max_chars) { buffer[i++] = '0'; count++; } ...
                    
                    // Copy number string
                    for (int j = 0; j < len && i < max_chars; j++) {
                        buffer[i++] = temp_buf[j];
                        count++;
                    }
                    break;
                }
                                
                case 'p': {  /* Pointer */
                    void* ptr_val = va_arg(args, void*);
                    uint32_t value = (uint32_t)ptr_val;
                    int len = utoa(value, temp_buf, 16);
                    int prefix_len = 2; // "0x"
                    // Pointers often have fixed width like 0x prefixed 8 hex digits
                    // Forcing min_width=8+prefix_len and zero padding might be desired
                    // min_width = (min_width > len + prefix_len) ? min_width : len + prefix_len;
                    if (min_width < len + prefix_len) min_width = len + prefix_len; 
                    is_zero_padded = 1; // Pointers usually zero padded
                    pad_char = '0';

                    int padding = min_width > (len + prefix_len) ? min_width - (len + prefix_len) : 0;
                    
                    // Output "0x" prefix first
                    if (i < max_chars) { buffer[i++] = '0'; count++; }
                    if (i < max_chars) { buffer[i++] = 'x'; count++; }
                    
                    // Apply zero padding BETWEEN prefix and number
                    for (int p = 0; p < padding && i < max_chars; p++) {
                        buffer[i++] = pad_char; 
                        count++;
                    }
                    
                    // Copy number string
                    for (int j = 0; j < len && i < max_chars; j++) {
                        buffer[i++] = temp_buf[j];
                        count++;
                    }
                    break;
                }
                
                case '%': {  /* Escaped % character */
                    if (i < max_chars) {
                        buffer[i++] = '%';
                        count++;
                    }
                    break;
                }
                
                default:  /* Unsupported format, just output as-is */
                    if (i < max_chars) {
                        buffer[i++] = '%';
                        count++;
                    }
                    if (i < max_chars && format[pos] != '\0') {
                        buffer[i++] = format[pos];
                        count++;
                    }
                    break;
            }
        } else {
            /* Regular character */
            if (i < max_chars) {
                buffer[i++] = format[pos];
                count++;
            }
        }
    }
    
    /* Ensure null-termination */
    buffer[i] = '\0'; 
    
    return count;
} 