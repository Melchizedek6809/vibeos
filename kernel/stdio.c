#include "include/stdio.h"
#include "include/stddef.h"
#include "include/stdint.h"
#include "include/stdarg.h"
#include "vga.h"
#include "serial.h"

/* Write a string to both terminal and serial outputs */
void puts(const char* str) {
    terminal_write(str);
    serial_write_string(str);
}

/* Internal function to reverse a string in place */
static void reverse_str(char* start, char* end) {
    char temp;
    while (start < end) {
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

/* Convert integer to string and return length */
static int itoa(int32_t value, char* str, int base) {
    int i = 0;
    int is_negative = 0;
    
    /* Handle 0 explicitly */
    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }
    
    /* Handle negative numbers for base 10 */
    if (value < 0 && base == 10) {
        is_negative = 1;
        value = -value;
    }
    
    /* Process individual digits */
    while (value != 0) {
        int remainder = value % base;
        str[i++] = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
        value /= base;
    }
    
    /* Add negative sign if needed */
    if (is_negative)
        str[i++] = '-';
    
    /* Null terminate the string */
    str[i] = '\0';
    
    /* Reverse the string */
    reverse_str(str, &str[i - 1]);
    
    return i;
}

/* Convert unsigned integer to string and return length */
static int utoa(uint32_t value, char* str, int base) {
    int i = 0;
    
    /* Handle 0 explicitly */
    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }
    
    /* Process individual digits */
    while (value != 0) {
        int remainder = value % base;
        str[i++] = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
        value /= base;
    }
    
    /* Null terminate the string */
    str[i] = '\0';
    
    /* Reverse the string */
    reverse_str(str, &str[i - 1]);
    
    return i;
}

/* Format a string and store it in the buffer */
int snprintf(char* buffer, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
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
    
    va_end(args);
    return count;
} 