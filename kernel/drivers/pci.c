#include "pci.h"
#include "../arch/x86/io.h"
#include "../stdlib/stdio.h" // For printf
#include "pci_db.h" // For human-readable names

// Helper function to create the 32-bit address for CONFIG_ADDRESS
static uint32_t pci_build_address(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    // Ensure the offset is aligned to 4 bytes for dword reads/writes
    // and extract the lower 2 bits for later use in word/byte reads.
    uint8_t offset_aligned = offset & 0xFC; // Mask out lower 2 bits

    // Construct the address packet
    uint32_t address = (uint32_t)((bus << 16) |      // Bus number
                              (device << 11) |   // Device number
                              (func << 8) |      // Function number
                              offset_aligned |   // Register offset (dword aligned)
                              (1 << 31));      // Enable bit (must be set)
    return address;
}

// Read a 32-bit DWORD from PCI configuration space
uint32_t pci_read_config_dword(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    // Check if the offset is dword-aligned. PCI requires this.
    if (offset & 0x03) {
        // Handle error: Offset not dword aligned
        // For now, return an error indicator or 0. Returning all FFs is common.
        // printf("PCI Read Error: Offset %x not dword aligned.\n", offset);
        return 0xFFFFFFFF;
    }
    
    uint32_t address = pci_build_address(bus, device, func, offset);

    // Write the address to the CONFIG_ADDRESS port
    outl(PCI_CONFIG_ADDRESS, address);

    // Read the data from the CONFIG_DATA port
    uint32_t result = inl(PCI_CONFIG_DATA);

    return result;
}

// Read a 16-bit WORD from PCI configuration space
uint16_t pci_read_config_word(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    // Read the containing DWORD
    uint32_t dword = pci_read_config_dword(bus, device, func, offset & 0xFC);
    
    // Extract the correct word based on the original offset's lower 2 bits
    // offset & 0x02 determines if we want the high word (offset 2 or 3) or low word (offset 0 or 1)
    return (uint16_t)((dword >> ((offset & 2) * 8)) & 0xFFFF);
}

// Read an 8-bit BYTE from PCI configuration space
uint8_t pci_read_config_byte(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) {
    // Read the containing DWORD
    uint32_t dword = pci_read_config_dword(bus, device, func, offset & 0xFC);
    
    // Extract the correct byte based on the original offset's lower 2 bits
    return (uint8_t)((dword >> ((offset & 3) * 8)) & 0xFF);
}

// Check if a device exists by reading its Vendor ID
uint16_t pci_check_device(uint8_t bus, uint8_t device, uint8_t func) {
    return pci_read_config_word(bus, device, func, PCI_VENDOR_ID_OFFSET);
}

// Enumerate the PCI bus(es)
void pci_enumerate_bus() {
    printf("\nPCI Bus Enumeration:\n");
    printf("%-8s | %-12s | %-20s | %-20s\n", "Bus:Dev", "Vendor", "Device", "Class");
    printf("-----------------------------------------------------------\n");

    // For simplicity, we'll only scan the first bus (bus 0) for now
    // A full implementation would scan buses 0-255
    for (uint8_t bus = 0; bus < 1; bus++) { // Simplified loop
        for (uint8_t device = 0; device < 32; device++) {
            uint8_t func = 0;
            uint16_t vendor_id = pci_check_device(bus, device, func);

            if (vendor_id == PCI_INVALID_VENDOR_ID) {
                continue; // No device here
            }

            // Read header type to check for multi-function devices
            uint8_t header_type = pci_read_config_byte(bus, device, func, PCI_HEADER_TYPE_OFFSET);
            uint8_t max_funcs = (header_type & PCI_MULTIFUNCTION_MASK) ? 8 : 1;

            // Iterate through functions if it's a multi-function device
            for (; func < max_funcs; func++) {
                vendor_id = pci_check_device(bus, device, func);
                if (vendor_id == PCI_INVALID_VENDOR_ID) {
                    continue; // No function here
                }

                // Read other details
                uint16_t device_id = pci_read_config_word(bus, device, func, PCI_DEVICE_ID_OFFSET);
                uint8_t class_code = pci_read_config_byte(bus, device, func, PCI_CLASS_CODE_OFFSET);
                uint8_t subclass_code = pci_read_config_byte(bus, device, func, PCI_SUBCLASS_CODE_OFFSET);

                // Get human-readable names
                const char* vendor_name = pci_vendor_name(vendor_id);
                const char* device_name = pci_device_name(vendor_id, device_id);
                const char* class_name = pci_class_name(class_code, subclass_code);

                // Format bus:device.function
                char location[9];
                snprintf(location, sizeof(location), "%02x:%02x.%x", bus, device, func);

                // Print device information with names
                printf("%-8s| %-12s | %-24s | %-20s\n",
                       location,
                       vendor_name,
                       device_name,
                       class_name);
            }
        }
    }
    printf("-----------------------------------------------------------\n");
    printf("PCI Enumeration Complete.\n");
} 