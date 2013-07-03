#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# @file: tools/mkboot.py
# Copyright (c) 2013 Korepwx. All rights reserved.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Contributors:
#   Korepwx  <public@korepwx.com>  2013-07-03
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

import sys

if (len(sys.argv) < 3):
  sys.stderr.write('Usage: <input filename> <output filename>\n')
  sys.exit(-1)
  
bootimg = None
with open(sys.argv[1], 'rb') as f:
  bootimg = f.read()
  
if (len(bootimg) > 510):
  sys.stderr.write('BootLoader exceeds 510 bytes (actually %s bytes)!\n' % 
                   len(bootimg))
  sys.exit(-1)
  
bootimg = bootimg + '\0' * (510 - len(bootimg))
bootimg += '\x55\xAA'

with open(sys.argv[2], 'wb') as f:
  f.write(bootimg)

print ('Build 512 bytes boot sector: "%s" successfully!' % sys.argv[2])
