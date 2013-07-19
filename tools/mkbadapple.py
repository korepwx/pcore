#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# @file: tools/mkbadapple.py
# Copyright (c) 2013 Korepwx. All rights reserved.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Contributors:
#   Korepwx  <public@korepwx.com>  2013-07-18
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

# Embed badapple video data into kernel .
badapple_video = '../pcore-badapple/video/badapple.dat'
badapple_csrc  = 'kern/badapple/badapple_video.c'

with open(badapple_csrc, 'wb') as cf:
  cf.write('#include <stddef.h>\n\nuint8_t badapple_compressed_video[] = {\n')
  with open(badapple_video, 'rb') as vf:
    cnt = vf.read()
    for i in range(0, len(cnt)):
      cf.write('%s,' % ord(cnt[i]))
      if (i % 32 == 0):
        cf.write('\n')
  cf.write('\n0,0,0,0\n};\n')
