# Multiboot header - required for proper boot
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum of above to prove we are multiboot

# The actual multiboot header
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Setup a simple stack
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

# Kernel entry point
.section .text
.global _start
.type _start, @function
_start:
    # Setup the stack
    mov $stack_top, %esp
    
    # Reset EFLAGS
    push $0
    popf
    
    # Call the kernel main function with multiboot info
    # eax contains the magic number
    # ebx contains the address of the multiboot info structure
    push %ebx  # multiboot_info_t*
    push %eax  # multiboot magic
    call kernel_main
    
    # In case kernel_main returns, halt the CPU
    cli      # Disable interrupts
    hlt      # Halt the CPU
1:  jmp 1b   # Infinite loop if hlt doesn't work

# Make a global reference to the entry point so the linker can find it
.size _start, . - _start
.global start
start:
    jmp _start 