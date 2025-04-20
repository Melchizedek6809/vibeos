#ifndef PIC_H
#define PIC_H

#include "include/stdint.h"

/* PIC I/O ports */
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

/* PIC commands */
#define PIC_EOI         0x20    /* End of Interrupt */

#define ICW1_ICW4       0x01    /* ICW4 needed */
#define ICW1_SINGLE     0x02    /* Single cascade mode */
#define ICW1_INTERVAL4  0x04    /* Call address interval 4 */
#define ICW1_LEVEL      0x08    /* Level triggered mode */
#define ICW1_INIT       0x10    /* Initialization */

#define ICW4_8086       0x01    /* 8086/88 mode */
#define ICW4_AUTO       0x02    /* Auto EOI */
#define ICW4_BUF_SLAVE  0x08    /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C    /* Buffered mode/master */
#define ICW4_SFNM       0x10    /* Special fully nested */

/* Remap the PIC interrupt numbers to avoid conflicts with CPU exceptions */
void pic_remap(uint8_t offset1, uint8_t offset2);

/* Send End-of-Interrupt signal to the PIC controllers */
void pic_send_eoi(uint8_t irq);

/* Disable all interrupts in the PIC */
void pic_disable(void);

/* Enable a specific IRQ line */
void pic_enable_irq(uint8_t irq);

/* Disable a specific IRQ line */
void pic_disable_irq(uint8_t irq);

#endif /* PIC_H */ 