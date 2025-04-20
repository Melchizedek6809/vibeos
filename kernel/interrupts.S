.section .text
.global idt_load

/* Load the IDT register from the provided address */
idt_load:
    mov 4(%esp), %eax    /* Get the pointer to IDTR */
    lidt (%eax)          /* Load the IDT register */
    ret

/* Macro to create an ISR that does not push an error code */
.macro ISR_NOERRCODE num
.global isr\num
isr\num:
    cli                  /* Disable interrupts */
    push $0              /* Push dummy error code */
    push $\num           /* Push interrupt number */
    jmp isr_common       /* Jump to common handler */
.endm

/* Macro to create an ISR that pushes an error code */
.macro ISR_ERRCODE num
.global isr\num
isr\num:
    cli                  /* Disable interrupts */
    push $\num           /* Push interrupt number (error code is pushed automatically) */
    jmp isr_common       /* Jump to common handler */
.endm

/* Common ISR handler code - calls C function */
isr_common:
    /* Save all registers */
    pusha

    /* Call C handler */
    push 36(%esp)        /* Push interrupt number as argument */
    call isr_handler
    add $4, %esp         /* Clean up the argument */

    /* Restore registers */
    popa
    add $8, %esp         /* Clean up error code and interrupt number */
    sti                  /* Enable interrupts */
    iret                 /* Return from interrupt */

/* Define ISRs for exceptions (0-31) */
ISR_NOERRCODE 0    /* Division by zero */
ISR_NOERRCODE 1    /* Debug */
ISR_NOERRCODE 2    /* Non-maskable interrupt */
ISR_NOERRCODE 3    /* Breakpoint */
ISR_NOERRCODE 4    /* Overflow */
ISR_NOERRCODE 5    /* Bound range exceeded */
ISR_NOERRCODE 6    /* Invalid opcode */
ISR_NOERRCODE 7    /* Device not available */
ISR_ERRCODE   8    /* Double fault */
ISR_NOERRCODE 9    /* Coprocessor segment overrun */
ISR_ERRCODE   10   /* Invalid TSS */
ISR_ERRCODE   11   /* Segment not present */
ISR_ERRCODE   12   /* Stack-segment fault */
ISR_ERRCODE   13   /* General protection fault */
ISR_ERRCODE   14   /* Page fault */
ISR_NOERRCODE 15   /* Reserved */
ISR_NOERRCODE 16   /* x87 floating-point exception */
ISR_ERRCODE   17   /* Alignment check */
ISR_NOERRCODE 18   /* Machine check */
ISR_NOERRCODE 19   /* SIMD floating-point exception */
ISR_NOERRCODE 20   /* Virtualization exception */
ISR_NOERRCODE 21   /* Reserved */
ISR_NOERRCODE 22   /* Reserved */
ISR_NOERRCODE 23   /* Reserved */
ISR_NOERRCODE 24   /* Reserved */
ISR_NOERRCODE 25   /* Reserved */
ISR_NOERRCODE 26   /* Reserved */
ISR_NOERRCODE 27   /* Reserved */
ISR_NOERRCODE 28   /* Reserved */
ISR_NOERRCODE 29   /* Reserved */
ISR_ERRCODE   30   /* Security exception */
ISR_NOERRCODE 31   /* Reserved */

/* Define handlers for custom interrupts (32+) */
ISR_NOERRCODE 32   /* Custom interrupt 0 */
ISR_NOERRCODE 33   /* Custom interrupt 1 */
ISR_NOERRCODE 34   /* Custom interrupt 2 */
ISR_NOERRCODE 35   /* Custom interrupt 3 */
ISR_NOERRCODE 36   /* Custom interrupt 4 */
ISR_NOERRCODE 37   /* Custom interrupt 5 */
ISR_NOERRCODE 38   /* Custom interrupt 6 */
ISR_NOERRCODE 39   /* Custom interrupt 7 */
ISR_NOERRCODE 40   /* Custom interrupt 8 */
ISR_NOERRCODE 41   /* Custom interrupt 9 */
ISR_NOERRCODE 42   /* Custom interrupt 10 */
ISR_NOERRCODE 43   /* Custom interrupt 11 */
ISR_NOERRCODE 44   /* Custom interrupt 12 */
ISR_NOERRCODE 45   /* Custom interrupt 13 */
ISR_NOERRCODE 46   /* Custom interrupt 14 */
ISR_NOERRCODE 47   /* Custom interrupt 15 */ 