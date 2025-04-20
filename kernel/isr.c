#include <stdint.h>
#include "include/idt.h"
#include "include/stdio.h"
#include "serial.h"
#include "vga.h"

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
    /* For debugging: Print the interrupt that occurred */
    printf("Received interrupt: %d", regs.int_no);
    
    /* If it's an exception (0-31), print more detail */
    if (regs.int_no < 32) {
        printf(" - %s Exception", exception_messages[regs.int_no]);
        
        /* For some exceptions, print the error code too */
        if (regs.int_no == 14) { /* Page fault */
            printf("\nError Code: %x", regs.err_code);
            /* Bit 0: Present - 0=non-present page, 1=protection violation
               Bit 1: Write - 0=read, 1=write
               Bit 2: User - 0=supervisor, 1=user
               Bit 3: Reserved write - 0=not reserved bit violation, 1=reserved bit violation
               Bit 4: Instruction Fetch - 0=not instruction fetch, 1=instruction fetch */
            printf("\nFault was caused by a %s", (regs.err_code & 0x1) ? "page-level protection violation" : "non-present page");
            printf("\nAccess type: %s", (regs.err_code & 0x2) ? "write" : "read");
            printf("\nProcessor mode: %s", (regs.err_code & 0x4) ? "user-mode" : "supervisor-mode");
        }
        
        /* Debug output to serial port as well */
        printf("\nEXCEPTION: %s (INT %d)\n", exception_messages[regs.int_no], regs.int_no);
        
        /* Serious error - halt the system */
        printf("\nSystem Halted!");
        for(;;); /* Infinite loop */
    }
    
    /* Check if we have a custom handler for this interrupt */
    if (interrupt_handlers[regs.int_no] != 0) {
        isr_handler_t handler = interrupt_handlers[regs.int_no];
        handler(&regs);
    } else if (regs.int_no >= 32) {
        /* If no handler for non-exception interrupt, just acknowledge it */
        printf("Unhandled interrupt: %d\n", regs.int_no);
    }
} 