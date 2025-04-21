#include "../include/drivers/pci_db.h"

const char* pci_vendor_name(uint16_t vendor_id) {
    for (const pci_vendor_info_t* vendor = pci_vendors; vendor->name != NULL; vendor++) {
        if (vendor->id == vendor_id) {
            return vendor->name;
        }
    }
    return "Unknown Vendor";
}

const char* pci_device_name(uint16_t vendor_id, uint16_t device_id) {
    for (const pci_device_info_t* device = pci_devices; device->name != NULL; device++) {
        if (device->vendor_id == vendor_id && device->device_id == device_id) {
            return device->name;
        }
    }
    return "Unknown Device";
}

const char* pci_class_name(uint8_t class_code, uint8_t subclass) {
    for (const pci_class_info_t* class = pci_classes; class->name != NULL; class++) {
        if (class->class_code == class_code && class->subclass == subclass) {
            return class->name;
        }
    }
    return "Unknown Class";
} 