#include <stdint.h>
#include "include/idt.h"
#include "include/stdio.h"
#include "serial.h"
#include "vga.h"
#include "pic.h"

/* Array of function pointers to custom interrupt handlers */
isr_handler_t interrupt_handlers[IDT_ENTRIES];

/* Names of the first 32 exceptions for debug output */
const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

/* Register a custom handler for an interrupt */
void register_interrupt_handler(uint8_t n, isr_handler_t handler) {
    interrupt_handlers[n] = handler;
}

/* Main interrupt service routine handler
 * This gets called from our assembly interrupt handler stub */
void isr_handler(registers_t regs) {
    /* If it's an exception (0-31), print more detail */
    if (regs.int_no < 32) {
        printf("\033[1;31mEXCEPTION: %s (INT %d)\033[0m\n", exception_messages[regs.int_no], regs.int_no);
        
        /* For some exceptions, print the error code too */
        if (regs.int_no == 14) { /* Page fault */
            printf("Error Code: %x\n", regs.err_code);
            printf("Fault was caused by a %s\n", (regs.err_code & 0x1) ? "page-level protection violation" : "non-present page");
            printf("Access type: %s\n", (regs.err_code & 0x2) ? "write" : "read");
            printf("Processor mode: %s\n", (regs.err_code & 0x4) ? "user-mode" : "supervisor-mode");
        }
        
        /* Serious error - halt the system */
        printf("\033[1;31mSystem Halted!\033[0m\n");
        for(;;); /* Infinite loop */
    }
    
    /* Check if we have a custom handler for this interrupt */
    if (interrupt_handlers[regs.int_no] != 0) {
        isr_handler_t handler = interrupt_handlers[regs.int_no];
        handler(&regs);
    }
    
    /* Send EOI if this was an IRQ (32-47) */
    if (regs.int_no >= 32 && regs.int_no < 48) {
        pic_send_eoi(regs.int_no - 32);
    }
} 