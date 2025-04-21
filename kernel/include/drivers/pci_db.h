#ifndef KERNEL_PCI_DB_H
#define KERNEL_PCI_DB_H

#include "../stdint.h"
#include "../stddef.h"  // For NULL definition

/* Common PCI Vendor IDs */
typedef struct {
    uint16_t id;
    const char* name;
} pci_vendor_info_t;

static const pci_vendor_info_t pci_vendors[] = {
    {0x1022, "AMD"},
    {0x106B, "Apple"},
    {0x1234, "Bochs"},
    {0x1274, "Ensoniq"},
    {0x15AD, "VMware"},
    {0x1AF4, "Red Hat"},
    {0x8086, "Intel"},
    {0x10DE, "NVIDIA"},
    {0x1002, "AMD"},
    {0x1013, "Cirrus Logic"},
    {0x1414, "Microsoft"},
    {0x1B36, "QEMU"},
    {0, (const char*)NULL} // Terminator
};

/* Common Device Classes */
typedef struct {
    uint8_t class_code;
    uint8_t subclass;
    const char* name;
} pci_class_info_t;

static const pci_class_info_t pci_classes[] = {
    // Mass Storage Controllers
    {0x01, 0x00, "SCSI Bus Controller"},
    {0x01, 0x01, "IDE Controller"},
    {0x01, 0x06, "SATA Controller"},
    {0x01, 0x08, "NVMe Controller"},
    // Network Controllers
    {0x02, 0x00, "Ethernet Controller"},
    {0x02, 0x80, "Network Controller"},
    // Display Controllers
    {0x03, 0x00, "VGA Compatible Controller"},
    {0x03, 0x01, "XGA Controller"},
    {0x03, 0x02, "3D Controller"},
    // Multimedia Controllers
    {0x04, 0x00, "Multimedia Video Controller"},
    {0x04, 0x01, "Multimedia Audio Controller"},
    {0x04, 0x03, "Audio Device"},
    // Bridge Devices
    {0x06, 0x00, "Host Bridge"},
    {0x06, 0x01, "ISA Bridge"},
    {0x06, 0x04, "PCI-to-PCI Bridge"},
    {0x06, 0x80, "Bridge Device"},
    // Communication Controllers
    {0x07, 0x00, "Serial Controller"},
    {0x07, 0x01, "Parallel Controller"},
    {0x07, 0x03, "Modem"},
    // System Peripherals
    {0x08, 0x00, "PIC"},
    {0x08, 0x01, "DMA Controller"},
    {0x08, 0x02, "Timer"},
    {0x08, 0x03, "RTC Controller"},
    // Input Devices
    {0x09, 0x00, "Keyboard Controller"},
    {0x09, 0x02, "Mouse Controller"},
    // Docking Stations
    {0x0A, 0x00, "Docking Station"},
    // Processors
    {0x0B, 0x00, "386"},
    {0x0B, 0x01, "486"},
    {0x0B, 0x02, "Pentium"},
    {0x0B, 0x10, "Alpha"},
    {0x0B, 0x40, "Co-Processor"},
    // Serial Bus Controllers
    {0x0C, 0x00, "FireWire Controller"},
    {0x0C, 0x03, "USB Controller"},
    {0x0C, 0x05, "SMBus Controller"},
    // Wireless Controllers
    {0x0D, 0x00, "iRDA Controller"},
    {0x0D, 0x11, "RF Controller"},
    {0x0D, 0x20, "Bluetooth Controller"},
    {0x0D, 0x21, "Broadband Controller"},
    // Satellite Communication Controllers
    {0x0F, 0x01, "Satellite TV Controller"},
    {0x0F, 0x03, "Satellite Voice Controller"},
    {0x0F, 0x04, "Satellite Data Controller"},
    {0, 0, (const char*)NULL} // Terminator
};

/* Common Device IDs for some vendors */
typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    const char* name;
} pci_device_info_t;

static const pci_device_info_t pci_devices[] = {
    // Intel
    {0x8086, 0x100E, "Intel PRO/1000 Network Connection"},
    {0x8086, 0x1237, "Intel 440FX - 82441FX PMC"},
    {0x8086, 0x7000, "Intel 82371SB PIIX3 ISA"},
    {0x8086, 0x7010, "Intel 82371SB PIIX3 IDE"},
    {0x8086, 0x7020, "Intel 82371SB PIIX3 USB"},
    {0x8086, 0x7113, "Intel 82371AB/EB/MB PIIX4 ACPI"},
    // VMware
    {0x15AD, 0x0405, "VMware SVGA II Adapter"},
    {0x15AD, 0x0740, "VMware Virtual Machine Communication Interface"},
    {0x15AD, 0x0770, "VMware USB Controller"},
    {0x15AD, 0x0790, "VMware PCI Bridge"},
    // QEMU
    {0x1B36, 0x0001, "QEMU Virtual Video Controller"},
    {0x1B36, 0x0002, "QEMU Virtual Network Card"},
    {0x1B36, 0x0003, "QEMU Virtual Storage Controller"},
    // Red Hat
    {0x1AF4, 0x1000, "Virtio Network Device"},
    {0x1AF4, 0x1001, "Virtio Block Device"},
    {0x1AF4, 0x1002, "Virtio Memory Balloon"},
    {0x1AF4, 0x1003, "Virtio Console"},
    {0x1AF4, 0x1004, "Virtio SCSI"},
    {0x1AF4, 0x1005, "Virtio RNG"},
    {0x1AF4, 0x1009, "Virtio 9P Transport"},
    // Cirrus
    {0x1013, 0x00B8, "Cirrus Logic GD 5446"},
    {0, 0, (const char*)NULL} // Terminator
};

/* Function prototypes */
const char* pci_vendor_name(uint16_t vendor_id);
const char* pci_device_name(uint16_t vendor_id, uint16_t device_id);
const char* pci_class_name(uint8_t class_code, uint8_t subclass);

#endif // KERNEL_PCI_DB_H 