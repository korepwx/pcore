// @file: kern/dev/pci/pcidb.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-21
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <dev/pci/pci.h>
#include <sort.h>
#include "pcidb.h"

// Compare functions for binary search.
static inline int compare_vendor(const void* val, const void* key) 
{
  VendorData *a = (VendorData*)val;
  PCIDevice *b = (PCIDevice*)key;
  return (int)a->pci_vendor - (int)b->pci_vendor;
}

static inline int compare_vendor_device(const void* val, const void* key)
{
  VendorDeviceData *a = (VendorDeviceData*)val;
  PCIDevice *b = (PCIDevice*)key;
  int r1 = (int)a->pci_vendor - (int)b->pci_vendor;
  return (r1 == 0) ? ((int)a->pci_device - (int)b->pci_device) : r1;
}

static inline int compare_class(const void* val, const void* key)
{
  ClassData *a = (ClassData*)val;
  PCIDevice *b = (PCIDevice*)key;
  return (int)a->pci_class - (int)b->pci_class;
}

static inline int compare_subclass(const void* val, const void* key)
{
  SubclassData *a = (SubclassData*)val;
  PCIDevice *b = (PCIDevice*)key;
  int r1 = (int)a->pci_class - (int)b->pci_class;
  return (r1 == 0) ? ((int)a->pci_subclass - (int)b->pci_subclass) : r1;
}

// Query the names of PCI device.
void pci_query_names(PCIDevice* pci, const char** pci_vendor, 
                     const char** pci_device, const char** pci_class, 
                     const char** pci_subclass)
{
  // Query about the vendor.
  if (pci_vendor != NULL) {
    VendorData *vendor = 
      binary_find(vendor_data, 
                  karraysize(vendor_data), 
                  sizeof(vendor_data[0]), 
                  pci,
                  compare_vendor);
    *pci_vendor = (vendor != NULL) ? (vendor->vendor_name) : NULL;
  }
  
  // Query about the device.
  if (pci_device != NULL) {
    VendorDeviceData *device = 
      binary_find(vendor_device_data, 
                  karraysize(vendor_device_data), 
                  sizeof(vendor_device_data[0]), 
                  pci,
                  compare_vendor_device);
    *pci_device = (device != NULL) ? (device->device_name) : NULL;
  }
  
  // Query about the class.
  if (pci_class != NULL) {
    ClassData *_class = 
      binary_find(class_data, 
                  karraysize(class_data), 
                  sizeof(class_data[0]), 
                  pci,
                  compare_class);
    *pci_class = (_class != NULL) ? (_class->class_name) : NULL;
  }
  
  // Query about the subclass.
  if (pci_subclass != NULL) {
    SubclassData *subclass = 
      binary_find(subclass_data, 
                  karraysize(subclass_data), 
                  sizeof(subclass_data[0]), 
                  pci,
                  compare_subclass);
    *pci_subclass = (subclass != NULL) ? (subclass->subclass_name) : NULL;
  }
}
