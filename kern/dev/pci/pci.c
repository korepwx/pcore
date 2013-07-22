// @file: kern/dev/pci/pci.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-20
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <asm/io.h>
#include <dev/pci/pci.h>
#include <pcore/kmalloc.h>

// ---- The following utilites are used for pci list ----
static KListEntry pci_list;

static inline PCIDevice *pci_get_next_entry(KListEntry *itm) {
  KListEntry *le = klist_next(itm);
  if (le == &pci_list) 
    return NULL;
  return ktostruct(le, PCIDevice, pci_list);
}


PCIDevice *pci_first(void) {
  return pci_get_next_entry(&pci_list);
}
PCIDevice *pci_list_open(void) {
  return pci_get_next_entry(&pci_list);
}
PCIDevice *pci_list_next(PCIDevice* pci) {
  return pci_get_next_entry(&(pci->pci_list));
}
void pci_list_close(void) {
  // TODO: implement RW lock or RCU lock for pci_list.
}
void pci_list_add(PCIDevice* pci) {
  klist_add(&(pci_list), &(pci->pci_list));
}

// ---- Find or get a particular pci device ----
PCIDevice *pci_find(uint8_t vendor, uint8_t device)
{
  KListEntry *le = klist_next(&pci_list);
  while (le != &pci_list) {
    PCIDevice *d = to_struct(le, PCIDevice, pci_list);
    if (d->pci_vendor == vendor && d->pci_device == device)
      return d;
    le = klist_next(le);
  }
  return NULL;
}

PCIDevice *pci_device(uint8_t bus, uint8_t slot, uint8_t func)
{
  KListEntry *le = klist_next(&pci_list);
  while (le != &pci_list) {
    PCIDevice *d = to_struct(le, PCIDevice, pci_list);
    if (d->pci_bus == bus && d->pci_slot == slot && d->pci_func == func)
      return d;
    le = klist_next(le);
  }
  return NULL;
}

// ---- The following utilities are used for pci_init ----
// see: http://wiki.osdev.org/PCI
#define PCI_CONFIG_ADDRESS  0xCF8
#define PCI_CONFIG_DATA     0xCFC
#define PCI_INVALID_WORD    0xFFFF
#define PCI_INVALID_DWORD   0xFFFFFFFFU
#define PCI_ADDRESS(BUS, SLOT, FUNC, OFFSET) (0x80000000U | \
  ((BUS) << 16) | ((SLOT) << 11) | ((FUNC) << 8) | ((OFFSET) & 0xfc))

static inline uint32_t pci_read_config_uint
  (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
  /*
   * 31       enable bit
   * 30-24    reserved
   * 23-16    bus number      [0~255]
   * 15-11    slot number     [0~31]
   * 10-8     function number [0~7]
   * 7-2      register number
   * 1-0      00
   */

  uint32_t v = PCI_ADDRESS(bus, slot, func, offset);
  outl(PCI_CONFIG_ADDRESS, v);
  v = inl(PCI_CONFIG_DATA);
  outl(PCI_CONFIG_ADDRESS, 0x0);    // Disable read/write after this cycle.
  return v;
}

static inline void pci_write_config_uint
  (uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t val)
{
  uint32_t address = PCI_ADDRESS(bus, slot, func, offset);
  outl(PCI_CONFIG_ADDRESS, address);
  outl(PCI_CONFIG_DATA, val);
  outl(PCI_CONFIG_ADDRESS, 0x0);    // Disable read/write after this cycle.
}

#define PCI_READ_UINT16(BUS, SLOT, FUNC, OFFSET, V1, V0) ({ \
  uint32_t ret = pci_read_config_uint((BUS), (SLOT), (FUNC), (OFFSET)); \
  *(V1) = (uint16_t)((ret >> 16) & 0xffff); \
  *(V0) = (uint16_t)(ret & 0xffff); \
  ret != PCI_INVALID_DWORD; \
})

#define PCI_READ_UINT8(BUS, SLOT, FUNC, OFFSET, V3, V2, V1, V0) ({ \
  uint32_t ret = pci_read_config_uint((BUS), (SLOT), (FUNC), (OFFSET)); \
  *(V3) = (uint8_t)((ret >> 24) & 0xff); \
  *(V2) = (uint8_t)((ret >> 16) & 0xff); \
  *(V1) = (uint8_t)((ret >> 8) & 0xff); \
  *(V0) = (uint8_t)((ret) & 0xff); \
  ret != PCI_INVALID_DWORD; \
})

#define GET_VENDOR_DEVICE(BUS, SLOT, FUNC, VENDOR, DEVICE) \
  PCI_READ_UINT16((BUS), (SLOT), (FUNC), 0x00, (DEVICE), (VENDOR))

// Discover one device.
static void pci_got_device(uint8_t bus, uint8_t slot, uint8_t func,
                           uint16_t vendor, uint16_t device) {
  // Add this device into pci list.
  PCIDevice *d = kmalloc(sizeof(PCIDevice));
  memset(d, 0, sizeof(*d));
  
  // device location.
  d->pci_bus = bus;
  d->pci_slot = slot;
  d->pci_func = func;
  
  // common header data.
  d->pci_vendor = vendor;
  d->pci_device = device;
  PCI_READ_UINT8(bus, slot, func, 0x08, &(d->pci_class), &(d->pci_subclass),
                 &(d->pci_prog_if), &(d->pci_revision));
  pci_list_add(d);
  
  // Report vendor:device.
  const char *vendor_name, *device_name, *class_name, *subclass_name;
  pci_query_names(d, &vendor_name, &device_name, &class_name, &subclass_name);
  
  if (vendor_name != NULL || device_name != NULL || class_name != NULL 
      || subclass_name != NULL) {
    printf("  %02x:%02x.%d %s: %s %s\n", 
          bus, slot, func,
          (subclass_name != NULL) ? (subclass_name) : (
              (class_name != NULL) ? (class_name) : ("unknown type")
          ),
          (vendor_name != NULL) ? (vendor_name) : ("unknown vendor"),
          (device_name != NULL) ? (device_name) : ("unknown device")
    );
  } else {
    printf("  %02x:%02x.%d: %04X %04X\n", bus, slot, func, vendor, device);
  }
  
}

// Check one device at (bus, slot)
static void pci_check_device(uint8_t bus, uint8_t slot) {
  uint8_t func = 0;
  
  // Check main function at (bus, slot).
  uint16_t vendor, device;
  if (!GET_VENDOR_DEVICE(bus, slot, func, &vendor, &device))
    return;
  pci_got_device(bus, slot, func, vendor, device);
  
  for (func=1; func<8; ++func) {
    if (GET_VENDOR_DEVICE(bus, slot, func, &vendor, &device))
      pci_got_device(bus, slot, func, vendor, device);
  }
}

// Initialize the PCI bus.
void pci_init(void)
{
  // Initialize the pci device list.
  klist_init(&(pci_list));
  
  // Enumerate all known pci devices.
  printf("[pci] Following PCI devices discovered:\n");
  uint16_t bus, slot;
  for (bus=0; bus<256; ++bus) {
    for (slot=0; slot<32; ++slot) {
      pci_check_device(bus, slot);
    } // for slot
  } // for bus
}

// ---- Wrappers for read/write config ----
uint32_t pci_read_config(PCIDevice *pci, uint8_t offset) {
  return pci_read_config_uint
    (pci->pci_bus, pci->pci_slot, pci->pci_func, offset);
}

void pci_write_config(PCIDevice *pci, uint8_t offset, uint32_t val)
{
  pci_write_config_uint
    (pci->pci_bus, pci->pci_slot, pci->pci_func, offset, val);
}
