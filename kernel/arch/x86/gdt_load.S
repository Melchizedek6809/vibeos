.code32 # Tell GAS we are using 32-bit code

.section .text
.global gdt_load
.global gdt_flush

# Function to load the GDT pointer
# Expects the address of the gdt_ptr structure on the stack (at 4(%esp))
gdt_load:
    movl 4(%esp), %eax # Get the address of the gdt_ptr structure
    lgdt (%eax)        # Load the GDT pointer (operand is the address)
    ret                # Return to the caller

# Function to flush segment registers after loading GDT
# Expects code_selector at 4(%esp) and data_selector at 8(%esp)
gdt_flush:
    # Get selectors from stack
    movw 8(%esp), %ax  # data_selector into %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    # Perform a far jump to reload CS
    # Push the return address (.Lflush label) and the code selector
    # Note: AT&T syntax for lret requires operand size suffix if ambiguous,
    # but here it implicitly uses the pushed values.
    pushl 4(%esp)      # Push code selector (from original stack position)
    pushl $.Lflush     # Push address of .Lflush label
    lret               # Far return (pops address and selector)

.Lflush: # Local label convention for GAS
    ret                # Normal return to the C caller 