#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# @file: tools/mkpcidb.py
# Copyright (c) 2013 Korepwx. All rights reserved.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Contributors:
#   Korepwx  <public@korepwx.com>  2013-07-21
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

# ---- PART A, generate pci name database ----
# Load PCI database.
db = list(open('tools/pci.ids', 'rb'))

vendors = []
vendor_devices = []
classes = []
subclasses = []

cur_vendor = None
cur_class = None

stage = 0
for l in db:
  if (l.startswith('# Vendors, devices and subsystems')):
    stage = 1
  elif (l.startswith('# List of known device classes, subclasses')):
    stage = 2
  if (l.startswith('#')):
    continue
  
  if (not l.strip()):
    continue
  
  # Deal with vendor, device and subsystems.
  if (stage == 1):
    if (l.startswith('\t\t')):
      pass
    elif (l.startswith('\t')):
      # device device_name
      if (cur_vendor is None):
        continue
      l = l.strip().split(' ', 1)
      l = [v.strip() for v in l if v.strip()]
      if (len(l) < 2):
        continue
      vendor_devices.append( (cur_vendor, l[0], l[1]) )
    else:
      # vendor vendor_name
      l = l.strip().split(' ', 1)
      l = [v.strip() for v in l if v.strip()]
      if (len(l) < 2):
        continue
      vendors.append( (l[0], l[1]) )
      cur_vendor = l[0]
      
  # Deal with classes, subclasses.
  if (stage == 2):
    if (l.startswith('\t\t')):
      pass
    elif (l.startswith('\t')):
      # subclass, subclass_name
      if (not cur_class):
        continue
      l = l.strip().split(' ', 1)
      l = [v.strip() for v in l if v.strip()]
      if (len(l) < 2):
        continue
      subclasses.append( (cur_class, l[0], l[1]) )
    elif (l.startswith('C ')):
      # C class class_name
      l = l.strip().split(' ', 2)
      l = [v.strip() for v in l if v.strip()]
      if (len(l) < 3):
        continue
      classes.append( (l[1], l[2], ) )
      cur_class = l[1]
      
# Sort the lists
vendors = sorted(vendors)
vendor_devices = sorted(vendor_devices)
classes = sorted(classes)
subclasses = sorted(subclasses)

# Output template.
TPL = """
typedef struct {
  uint16_t pci_vendor;
  const char* vendor_name;
} VendorData;

typedef struct {
  uint16_t pci_vendor;
  uint16_t pci_device;
  const char* device_name;
} VendorDeviceData;

typedef struct {
  uint8_t pci_class;
  const char* class_name;
} ClassData;

typedef struct {
  uint8_t pci_class;
  uint8_t pci_subclass;
  const char* subclass_name;
} SubclassData;

static VendorData vendor_data[] = {
%(vendor_data)s
};

static VendorDeviceData vendor_device_data[] = {
%(vendor_device_data)s
};

static ClassData class_data[] = {
%(class_data)s
};

static SubclassData subclass_data[] = {
%(subclass_data)s
};
"""
TPL = TPL.strip() + '\n'

# Generate output.
def SlashIt(x):
  x = x.replace('\xe2\x80\x90', '-')
  x = x.replace('\\', '\\\\')
  x = x.replace('"', '\\"')
  x = x.replace('?', '\\x3f')
  return x

vendor_data = '\n'.join([
  '  {0x%s, "%s"},' % (v[0], SlashIt(v[1]))
  for v in vendors
]);
vendor_device_data = '\n'.join([
  '  {0x%s, 0x%s, "%s"},' % (v[0], v[1], SlashIt(v[2]))
  for v in vendor_devices
]);
class_data = '\n'.join([
  '  {0x%s, "%s"},' % (v[0], SlashIt(v[1]))
  for v in classes
]);
subclass_data = '\n'.join([
  '  {0x%s, 0x%s, "%s"},' % (v[0], v[1], SlashIt(v[2]))
  for v in subclasses
]);

output = (TPL % {
  'vendor_data': vendor_data,
  'vendor_device_data': vendor_device_data,
  'class_data': class_data,
  'subclass_data': subclass_data
})

# Save output
with open('kern/dev/pci/pcidb.h', 'wb') as f:
  f.write(output)

# ---- PART B, generate pci driver database ----

