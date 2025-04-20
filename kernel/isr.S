[GLOBAL idt_load]
[EXTERN isr_handler]

; Load the IDT
idt_load:
    mov eax, [esp+4]  ; Get IDTR pointer passed as argument
    lidt [eax]        ; Load the IDT descriptor
    ret

; Common ISR stub that saves registers and calls C handler
%macro ISR_NOERRCODE 1
[GLOBAL isr%1]
isr%1:
    cli               ; Disable interrupts
    push 0            ; Push dummy error code
    push %1           ; Push interrupt number
    jmp isr_common    ; Jump to common handler
%endmacro

; ISR stub for exceptions that push error codes
%macro ISR_ERRCODE 1
[GLOBAL isr%1]
isr%1:
    cli               ; Disable interrupts
    push %1           ; Push interrupt number
    jmp isr_common    ; Jump to common handler
%endmacro

; Define the first 32 ISRs (CPU exceptions)
ISR_NOERRCODE 0   ; Divide by zero
ISR_NOERRCODE 1   ; Debug exception
ISR_NOERRCODE 2   ; Non-maskable interrupt
ISR_NOERRCODE 3   ; Breakpoint
ISR_NOERRCODE 4   ; Overflow
ISR_NOERRCODE 5   ; Bound range exceeded
ISR_NOERRCODE 6   ; Invalid opcode
ISR_NOERRCODE 7   ; Device not available
ISR_ERRCODE   8   ; Double fault
ISR_NOERRCODE 9   ; Coprocessor segment overrun
ISR_ERRCODE   10  ; Invalid TSS
ISR_ERRCODE   11  ; Segment not present
ISR_ERRCODE   12  ; Stack-segment fault
ISR_ERRCODE   13  ; General protection fault
ISR_ERRCODE   14  ; Page fault
ISR_NOERRCODE 15  ; Reserved
ISR_NOERRCODE 16  ; x87 floating-point exception
ISR_ERRCODE   17  ; Alignment check
ISR_NOERRCODE 18  ; Machine check
ISR_NOERRCODE 19  ; SIMD floating-point exception
ISR_NOERRCODE 20  ; Virtualization exception
ISR_NOERRCODE 21  ; Reserved
ISR_NOERRCODE 22  ; Reserved
ISR_NOERRCODE 23  ; Reserved
ISR_NOERRCODE 24  ; Reserved
ISR_NOERRCODE 25  ; Reserved
ISR_NOERRCODE 26  ; Reserved
ISR_NOERRCODE 27  ; Reserved
ISR_NOERRCODE 28  ; Reserved
ISR_NOERRCODE 29  ; Reserved
ISR_NOERRCODE 30  ; Security exception
ISR_NOERRCODE 31  ; Reserved

; Common ISR handler stub
isr_common:
    ; Save all registers
    pusha
    
    ; Save data segment
    mov ax, ds
    push eax
    
    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Call C handler
    push esp        ; Push pointer to registers_t structure
    call isr_handler
    add esp, 4      ; Clean up stack
    
    ; Restore data segment
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Restore registers
    popa
    
    ; Clean up error code and interrupt number
    add esp, 8
    
    ; Return from interrupt
    iret 