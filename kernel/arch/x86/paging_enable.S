.code32

.section .text
.global load_page_directory
.global enable_paging

# Function to load the physical address of the page directory into CR3
# Expects page_directory_addr on the stack (4(%esp))
load_page_directory:
    movl 4(%esp), %eax  # Get the address from the stack
    movl %eax, %cr3     # Load it into CR3
    ret

# Function to enable paging by setting the PG bit in CR0
enable_paging:
    movl %cr0, %eax     # Read current CR0 value
    orl $0x80000000, %eax # Set the PG bit (bit 31)
    movl %eax, %cr0     # Write the modified value back to CR0
    ret 