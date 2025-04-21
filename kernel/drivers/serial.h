#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/* Serial port status codes */
typedef enum {
    SERIAL_SUCCESS = 0,
    SERIAL_ERROR_TIMEOUT = -1,
    SERIAL_ERROR_INVALID_PORT = -2
} serial_status_t;

/* Serial port configuration */
typedef struct {
    uint16_t port;       /* Base I/O port (e.g., COM1: 0x3F8) */
    uint32_t baud_rate;  /* Baud rate (default: 38400) */
    uint8_t data_bits;   /* Data bits (5-8, default: 8) */
    uint8_t stop_bits;   /* Stop bits (1-2, default: 1) */
    uint8_t parity;      /* Parity (0: none, 1: odd, 2: even) */
} serial_config_t;

/* Default configuration for COM1 */
extern const serial_config_t SERIAL_DEFAULT_CONFIG;

/* Initialize the serial port
 * Returns: SERIAL_SUCCESS on success, error code otherwise
 * Note: Uses default configuration if config is NULL
 */
serial_status_t serial_init(const serial_config_t* config);

/* Write a character to the serial port
 * Returns: SERIAL_SUCCESS on success, error code otherwise
 * Note: Automatically sends \r when \n is encountered
 */
serial_status_t serial_write_char(char c);

/* Write a string to the serial port
 * Returns: SERIAL_SUCCESS on success, error code otherwise
 * Note: Automatically sends \r when \n is encountered
 */
serial_status_t serial_write_string(const char* str);

/* Printf style function for serial output
 * Returns: Number of characters written on success, negative error code otherwise
 * Note: Limited to 256 characters output
 */
int serial_printf(const char* format, ...);

/* Check if the serial port is ready to transmit
 * Returns: 1 if ready, 0 if not ready, negative error code on error
 */
int serial_is_transmit_ready(void);

#endif /* SERIAL_H */ 