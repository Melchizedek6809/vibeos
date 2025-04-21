#ifndef KERNEL_PCI_H
#define KERNEL_PCI_H

#include "../stdint.h"

// PCI Configuration Space Access Ports (Mechanism 1)
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

// Value returned for non-existent devices
#define PCI_INVALID_VENDOR_ID 0xFFFF

// Configuration Space Header Offsets
#define PCI_VENDOR_ID_OFFSET        0x00
#define PCI_DEVICE_ID_OFFSET        0x02
#define PCI_COMMAND_OFFSET          0x04
#define PCI_STATUS_OFFSET           0x06
#define PCI_REVISION_ID_OFFSET      0x08
#define PCI_PROG_IF_OFFSET          0x09
#define PCI_SUBCLASS_CODE_OFFSET    0x0A
#define PCI_CLASS_CODE_OFFSET       0x0B
#define PCI_CACHE_LINE_SIZE_OFFSET  0x0C
#define PCI_LATENCY_TIMER_OFFSET    0x0D
#define PCI_HEADER_TYPE_OFFSET      0x0E
#define PCI_BIST_OFFSET             0x0F

// Header Type 0x00 Specific Offsets
#define PCI_BAR0_OFFSET             0x10
#define PCI_BAR1_OFFSET             0x14
#define PCI_BAR2_OFFSET             0x18
#define PCI_BAR3_OFFSET             0x1C
#define PCI_BAR4_OFFSET             0x20
#define PCI_BAR5_OFFSET             0x24
#define PCI_SUBSYSTEM_VENDOR_ID_OFFSET 0x2C
#define PCI_SUBSYSTEM_ID_OFFSET     0x2E
#define PCI_INTERRUPT_LINE_OFFSET   0x3C
#define PCI_INTERRUPT_PIN_OFFSET    0x3D

// Header Type masks
#define PCI_HEADER_TYPE_MASK        0x7F
#define PCI_MULTIFUNCTION_MASK      0x80

// Function prototypes

/**
 * @brief Read a 32-bit value from PCI configuration space.
 * @param bus PCI bus number (0-255)
 * @param device PCI device number (0-31)
 * @param func PCI function number (0-7)
 * @param offset Register offset within the configuration space (must be 4-byte aligned)
 * @return The 32-bit value read.
 */
uint32_t pci_read_config_dword(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);

/**
 * @brief Read a 16-bit value from PCI configuration space.
 * @param bus PCI bus number (0-255)
 * @param device PCI device number (0-31)
 * @param func PCI function number (0-7)
 * @param offset Register offset within the configuration space.
 * @return The 16-bit value read.
 */
uint16_t pci_read_config_word(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);

/**
 * @brief Read an 8-bit value from PCI configuration space.
 * @param bus PCI bus number (0-255)
 * @param device PCI device number (0-31)
 * @param func PCI function number (0-7)
 * @param offset Register offset within the configuration space.
 * @return The 8-bit value read.
 */
uint8_t pci_read_config_byte(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);

/**
 * @brief Checks if a device exists at the specified bus, device, and function.
 * @param bus PCI bus number
 * @param device PCI device number
 * @param func PCI function number
 * @return Vendor ID if device exists, PCI_INVALID_VENDOR_ID otherwise.
 */
uint16_t pci_check_device(uint8_t bus, uint8_t device, uint8_t func);

/**
 * @brief Enumerates the PCI bus(es) and prints information about found devices.
 */
void pci_enumerate_bus();

#endif // KERNEL_PCI_H 