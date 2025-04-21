#include "serial.h"
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include "../arch/x86/io.h"

/* Serial port registers */
#define SERIAL_DATA         0   /* Data register (R/W) */
#define SERIAL_INT_ENABLE   1   /* Interrupt enable (R/W) */
#define SERIAL_BAUD_LSB     0   /* Baud rate divisor LSB (R/W) */
#define SERIAL_BAUD_MSB     1   /* Baud rate divisor MSB (R/W) */
#define SERIAL_INT_ID       2   /* Interrupt identification (R) / FIFO control (W) */
#define SERIAL_LINE_CTRL    3   /* Line control (R/W) */
#define SERIAL_MODEM_CTRL   4   /* Modem control (R/W) */
#define SERIAL_LINE_STATUS  5   /* Line status (R) */
#define SERIAL_MODEM_STATUS 6   /* Modem status (R) */
#define SERIAL_SCRATCH      7   /* Scratch register (R/W) */

/* Line control bits */
#define SERIAL_LCR_5BITS   0x00
#define SERIAL_LCR_6BITS   0x01
#define SERIAL_LCR_7BITS   0x02
#define SERIAL_LCR_8BITS   0x03
#define SERIAL_LCR_1STOP   0x00
#define SERIAL_LCR_2STOP   0x04
#define SERIAL_LCR_NO_PAR  0x00
#define SERIAL_LCR_ODD_PAR 0x08
#define SERIAL_LCR_EVN_PAR 0x18
#define SERIAL_LCR_DLAB    0x80

/* FIFO control bits */
#define SERIAL_FCR_ENABLE  0x01
#define SERIAL_FCR_CLEAR_RX 0x02
#define SERIAL_FCR_CLEAR_TX 0x04
#define SERIAL_FCR_TRIG_1  0x00
#define SERIAL_FCR_TRIG_4  0x40
#define SERIAL_FCR_TRIG_8  0x80
#define SERIAL_FCR_TRIG_14 0xC0

/* Default configuration */
const serial_config_t SERIAL_DEFAULT_CONFIG = {
    .port = 0x3F8,        /* COM1 */
    .baud_rate = 38400,
    .data_bits = 8,
    .stop_bits = 1,
    .parity = 0
};

/* Current configuration */
static serial_config_t current_config;

/* Calculate baud rate divisor */
static uint16_t calculate_divisor(uint32_t baud_rate) {
    return 115200 / baud_rate;
}

/* Check if transmit is empty with timeout */
int serial_is_transmit_ready(void) {
    uint16_t timeout = 10000;  /* Arbitrary timeout value */
    while (timeout--) {
        if (inb(current_config.port + SERIAL_LINE_STATUS) & 0x20) {
            return 1;
        }
    }
    return 0;
}

/* Initialize serial port */
serial_status_t serial_init(const serial_config_t* config) {
    /* Use default config if none provided */
    if (!config) {
        config = &SERIAL_DEFAULT_CONFIG;
    }
    
    /* Store configuration */
    current_config = *config;
    
    /* Validate configuration */
    if (config->data_bits < 5 || config->data_bits > 8 ||
        config->stop_bits < 1 || config->stop_bits > 2 ||
        config->parity > 2) {
        return SERIAL_ERROR_INVALID_PORT;
    }
    
    /* Calculate baud rate divisor */
    uint16_t divisor = calculate_divisor(config->baud_rate);
    
    /* Disable interrupts */
    outb(config->port + SERIAL_INT_ENABLE, 0x00);
    
    /* Set DLAB to access baud rate divisor */
    outb(config->port + SERIAL_LINE_CTRL, SERIAL_LCR_DLAB);
    
    /* Set baud rate */
    outb(config->port + SERIAL_BAUD_LSB, divisor & 0xFF);
    outb(config->port + SERIAL_BAUD_MSB, divisor >> 8);
    
    /* Set line parameters and disable DLAB */
    uint8_t lcr = 0;
    switch (config->data_bits) {
        case 5: lcr |= SERIAL_LCR_5BITS; break;
        case 6: lcr |= SERIAL_LCR_6BITS; break;
        case 7: lcr |= SERIAL_LCR_7BITS; break;
        case 8: lcr |= SERIAL_LCR_8BITS; break;
    }
    lcr |= (config->stop_bits == 2) ? SERIAL_LCR_2STOP : SERIAL_LCR_1STOP;
    switch (config->parity) {
        case 1: lcr |= SERIAL_LCR_ODD_PAR; break;
        case 2: lcr |= SERIAL_LCR_EVN_PAR; break;
        default: lcr |= SERIAL_LCR_NO_PAR; break;
    }
    outb(config->port + SERIAL_LINE_CTRL, lcr);
    
    /* Configure and enable FIFOs */
    outb(config->port + SERIAL_INT_ID, 
         SERIAL_FCR_ENABLE | SERIAL_FCR_CLEAR_RX | 
         SERIAL_FCR_CLEAR_TX | SERIAL_FCR_TRIG_14);
    
    /* Enable interrupts and set RTS/DTR */
    outb(config->port + SERIAL_MODEM_CTRL, 0x0B);
    
    return SERIAL_SUCCESS;
}

/* Write a character to serial port */
serial_status_t serial_write_char(char c) {
    /* Wait until we can send with timeout */
    if (!serial_is_transmit_ready()) {
        return SERIAL_ERROR_TIMEOUT;
    }
    
    /* Send the character */
    outb(current_config.port + SERIAL_DATA, c);
    
    /* If newline, also send carriage return */
    if (c == '\n') {
        if (!serial_is_transmit_ready()) {
            return SERIAL_ERROR_TIMEOUT;
        }
        outb(current_config.port + SERIAL_DATA, '\r');
    }
    
    return SERIAL_SUCCESS;
}

/* Write a string to serial port */
serial_status_t serial_write_string(const char* str) {
    serial_status_t status;
    
    for (size_t i = 0; str[i] != '\0'; i++) {
        status = serial_write_char(str[i]);
        if (status != SERIAL_SUCCESS) {
            return status;
        }
    }
    
    return SERIAL_SUCCESS;
}

/* Printf style function for serial output */
int serial_printf(const char* format, ...) {
    char buffer[256];
    va_list args;
    
    va_start(args, format);
    int result = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    if (result < 0) {
        return result;
    }
    
    serial_status_t status = serial_write_string(buffer);
    return (status == SERIAL_SUCCESS) ? result : (int)status;
} 