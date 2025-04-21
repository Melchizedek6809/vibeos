#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../drivers/vga.h"
#include "../drivers/serial.h"

/* Helper function to apply padding */
static void apply_padding(char* buffer, size_t* i, size_t* count, size_t max_chars, 
                         size_t padding, char pad_char) {
    for (size_t p = 0; p < padding && *i < max_chars; p++) {
        buffer[(*i)++] = pad_char;
        (*count)++;
    }
}

/* Helper function to copy string with bounds checking */
static void copy_str_bounded(char* buffer, size_t* i, size_t* count, size_t max_chars,
                           const char* str, size_t len) {
    for (size_t j = 0; j < len && *i < max_chars; j++) {
        buffer[(*i)++] = str[j];
        (*count)++;
    }
}

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
            size_t min_width = 0;
            int is_zero_padded = 0;
            int is_left_aligned = 0;

            // Check for left alignment flag
            if (format[pos] == '-') {
                is_left_aligned = 1;
                pos++;
            }

            // Check for zero-padding flag (only if not left-aligned)
            if (!is_left_aligned && format[pos] == '0') {
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
                    size_t padding = min_width > str_len ? min_width - str_len : 0;
                    
                    // For left alignment, print string first then padding
                    if (is_left_aligned) {
                        copy_str_bounded(buffer, &i, &count, max_chars, str, str_len);
                        apply_padding(buffer, &i, &count, max_chars, padding, ' ');
                    } else {
                        // Right alignment (default)
                        apply_padding(buffer, &i, &count, max_chars, padding, ' ');
                        copy_str_bounded(buffer, &i, &count, max_chars, str, str_len);
                    }
                    break;
                }
                
                case 'c': {  /* Character */
                    char c = (char)va_arg(args, int);
                    size_t padding = min_width > 1 ? min_width - 1 : 0;
                    
                    if (is_left_aligned) {
                        if (i < max_chars) {
                            buffer[i++] = c;
                            count++;
                        }
                        apply_padding(buffer, &i, &count, max_chars, padding, ' ');
                    } else {
                        apply_padding(buffer, &i, &count, max_chars, padding, ' ');
                        if (i < max_chars) {
                            buffer[i++] = c;
                            count++;
                        }
                    }
                    break;
                }
                
                case 'd':
                case 'i': {  /* Signed integer */
                    int value = va_arg(args, int);
                    size_t len = itoa(value, temp_buf, 10);
                    size_t padding = min_width > len ? min_width - len : 0;
                    
                    if (is_left_aligned) {
                        copy_str_bounded(buffer, &i, &count, max_chars, temp_buf, len);
                        apply_padding(buffer, &i, &count, max_chars, padding, ' ');
                    } else {
                        // Handle sign for zero padding
                        if (value < 0 && is_zero_padded) {
                            if (i < max_chars) buffer[i++] = '-';
                            temp_buf[0] = '0'; // Remove sign from temp_buf for padding logic
                        }
                        apply_padding(buffer, &i, &count, max_chars, padding, pad_char);
                        size_t start_idx = (value < 0 && is_zero_padded) ? 1 : 0;
                        copy_str_bounded(buffer, &i, &count, max_chars, temp_buf + start_idx, len - start_idx);
                    }
                    break;
                }
                
                case 'u':
                case 'x': {  /* Unsigned integer and Hexadecimal (lowercase) */
                    unsigned int value = va_arg(args, unsigned int);
                    int base = (format[pos] == 'u') ? 10 : 16;
                    size_t len = utoa(value, temp_buf, base);
                    size_t padding = min_width > len ? min_width - len : 0;
                    
                    if (is_left_aligned) {
                        copy_str_bounded(buffer, &i, &count, max_chars, temp_buf, len);
                        apply_padding(buffer, &i, &count, max_chars, padding, ' ');
                    } else {
                        apply_padding(buffer, &i, &count, max_chars, padding, pad_char);
                        copy_str_bounded(buffer, &i, &count, max_chars, temp_buf, len);
                    }
                    break;
                }
                                
                case 'p': {  /* Pointer */
                    void* ptr_val = va_arg(args, void*);
                    uint32_t value = (uint32_t)ptr_val;
                    size_t len = utoa(value, temp_buf, 16);
                    size_t prefix_len = 2; // "0x"
                    
                    if (min_width < len + prefix_len) min_width = len + prefix_len; 
                    size_t padding = min_width > (len + prefix_len) ? min_width - (len + prefix_len) : 0;
                    
                    if (is_left_aligned) {
                        // Output "0x" prefix first
                        if (i < max_chars) { buffer[i++] = '0'; count++; }
                        if (i < max_chars) { buffer[i++] = 'x'; count++; }
                        copy_str_bounded(buffer, &i, &count, max_chars, temp_buf, len);
                        apply_padding(buffer, &i, &count, max_chars, padding, ' ');
                    } else {
                        is_zero_padded = 1; // Pointers usually zero padded
                        pad_char = '0';
                        // Output "0x" prefix first
                        if (i < max_chars) { buffer[i++] = '0'; count++; }
                        if (i < max_chars) { buffer[i++] = 'x'; count++; }
                        apply_padding(buffer, &i, &count, max_chars, padding, pad_char);
                        copy_str_bounded(buffer, &i, &count, max_chars, temp_buf, len);
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