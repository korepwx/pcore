#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# @file: tools/mktrap.py
# Copyright (c) 2013 Korepwx. All rights reserved.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Contributors:
#   Korepwx  <public@korepwx.com>  2013-07-17
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

# Generate i386 vectors.S
with open('arch/i386/trap/vectors.S', 'wb') as f:
  f.write('.text\n.globl __alltraps\n')
  for i in range(0, 256):
    f.write('.globl __trapvector%(id)s\n__trapvector%(id)s:\n' % {'id': i})
    f.write('  pushl $0\n  pushl $%(id)s\n  jmp __alltraps\n' % {'id': i})
  f.write('\n')
  f.write('.data\n.globl __trapvectors\n__trapvectors:\n')
  for i in range(0, 255):
    f.write('  .long __trapvector%(id)s\n' % {'id': i})
