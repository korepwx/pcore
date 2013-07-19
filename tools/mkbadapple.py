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
  
import os
  
# badapple config information.
video_data_start = 50 * 1024 * 1024  # 50M.
video_data_size  = os.path.getsize('video/badapple.dat')

# Make loading image.
badapple_loading = 'video/loading.dat'
badapple_loading_csrc = 'kern/badapple/badapple_loading.c'

with open(badapple_loading_csrc, 'wb') as cf:
  cf.write('#include <stddef.h>\n\nuint8_t badapple_loading_image[] = {\n')
  with open(badapple_loading, 'rb') as vf:
    cnt = vf.read()
    for i in xrange(0, len(cnt)):
      cf.write('%s,' % ord(cnt[i]))
      if (i % 32 == 31):
        cf.write('\n')
  cf.write('\n};\n')
  
# Generate header.
badapple_header = 'include/pcore/badapple_config.h'
with open(badapple_header, 'wb') as cf:
  cf.write('#ifndef __BADAPPLE_CONFIG_H\n#define __BADAPPLE_CONFIG_H\n\n')
  cf.write('#define BADAPPLE_VIDEO_DATA_SIZE   %s\n' % video_data_size)
  cf.write('#define BADAPPLE_VIDEO_DATA_OFFSET %s\n' % video_data_start)
  cf.write('\nextern uint8_t badapple_loading_image[];\n')
  cf.write('\n#endif\n')

# Embed badapple video data into kernel .
# NOTE: this part is abandoned.
def ABANDON_1():
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
