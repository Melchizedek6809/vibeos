#include "pic.h"
#include "io.h"
#include "../../include/stdint.h"

/* Remap the PIC interrupt numbers to avoid conflicts with CPU exceptions */
void pic_remap(uint8_t offset1, uint8_t offset2) {
    /* Save the masks */
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);
    
    /* Start initialization sequence (cascade mode) */
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    
    /* Set the vector offsets */
    outb(PIC1_DATA, offset1);     /* ICW2: Master PIC vector offset */
    outb(PIC2_DATA, offset2);     /* ICW2: Slave PIC vector offset */
    
    /* Set up cascading */
    outb(PIC1_DATA, 4);           /* ICW3: tell Master PIC there is a slave at IRQ2 (0000 0100) */
    outb(PIC2_DATA, 2);           /* ICW3: tell Slave PIC its cascade identity (0000 0010) */
    
    /* Set the mode */
    outb(PIC1_DATA, ICW4_8086);   /* ICW4: 8086 mode, normal EOI */
    outb(PIC2_DATA, ICW4_8086);   /* ICW4: 8086 mode, normal EOI */
    
    /* Restore masks */
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

/* Send End-of-Interrupt signal to the PIC controllers */
void pic_send_eoi(uint8_t irq) {
    /* If this was from a slave PIC irq (8-15), 
     * we need to send an EOI to both PICs */
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    
    /* Always send EOI to master PIC */
    outb(PIC1_COMMAND, PIC_EOI);
}

/* Disable all interrupts in the PIC */
void pic_disable(void) {
    /* Mask all interrupts */
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

/* Enable a specific IRQ line */
void pic_enable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t mask;
    
    if (irq < 8) {
        /* IRQ on master PIC */
        port = PIC1_DATA;
        mask = inb(port) & ~(1 << irq);
    } else {
        /* IRQ on slave PIC */
        port = PIC2_DATA;
        mask = inb(port) & ~(1 << (irq - 8));
    }
    
    outb(port, mask);
}

/* Disable a specific IRQ line */
void pic_disable_irq(uint8_t irq) {
    uint16_t port;
    uint8_t mask;
    
    if (irq < 8) {
        /* IRQ on master PIC */
        port = PIC1_DATA;
        mask = inb(port) | (1 << irq);
    } else {
        /* IRQ on slave PIC */
        port = PIC2_DATA;
        mask = inb(port) | (1 << (irq - 8));
    }
    
    outb(port, mask);
} 