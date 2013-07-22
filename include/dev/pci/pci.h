// @file: include/dev/pci/pci.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-20
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_DEV_PCI_PCI_H_7F430B36F14411E28C4A74E50BEE6214
#define _INCLUDE_DEV_PCI_PCI_H_7F430B36F14411E28C4A74E50BEE6214
#pragma once

#include <stddef.h>
#include <pcore/list.h>

/// @brief PCI device interface.
struct _PCIDevice {
  // ---- Device location on PCI bus ----
  uint8_t pci_bus;
  uint8_t pci_slot;
  uint8_t pci_func;
  uint8_t reserved0;  // Pad to 4 bytes.
  
  // ---- Common config to all headers ----
  // offset 0x00
  uint16_t pci_device;
  uint16_t pci_vendor;
  // offset 0x08
  uint8_t pci_class;
  uint8_t pci_subclass;
  uint8_t pci_prog_if;
  uint8_t pci_revision;
  
  // ---- PCI device list entry in kernel ----
  KListEntry pci_list;
};
typedef struct _PCIDevice PCIDevice;

/// @brief Initialize the PCI bus.
void pci_init(void);

/// @brief Find a particular PCI device with vendor, device
PCIDevice *pci_find(uint8_t vendor, uint8_t device);

/// @brief Get a particular PCI device at (bus, slot, func).
PCIDevice *pci_device(uint8_t bus, uint8_t slot, uint8_t func);

/// @brief Read PCI config dword value.
uint32_t pci_read_config(PCIDevice *pci, uint8_t offset);
  
/// @brief Write PCI config dword value.
void pci_write_config(PCIDevice *pci, uint8_t offset, uint32_t val);

/**
 * @brief Get vendor, device, class & subclass names for pci device.
 */
void pci_query_names(PCIDevice* pci, const char** pci_vendor, 
                     const char** pci_device, const char** pci_class, 
                     const char** pci_subclass);

// ---- Manipulate PCI device list ----
// @sa dev/fb/fb.h
PCIDevice *pci_first(void);
PCIDevice *pci_list_open(void);
PCIDevice *pci_list_next(PCIDevice* pci);
void pci_list_close(void);

#endif // _INCLUDE_DEV_PCI_PCI_H_7F430B36F14411E28C4A74E50BEE6214
