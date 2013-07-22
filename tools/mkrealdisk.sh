#!/bin/bash
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# @file: tools/mkrealdisk.sh
# Copyright (c) 2013 Korepwx. All rights reserved.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Contributors:
#   Korepwx  <public@korepwx.com>  2013-07-20
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file.

# Check environment.
if [ "`id -u`" != "0" ]; then
  echo "Only root can run this script."
  exit 1
fi

if [ ! -f "video/badapple.dat" ]; then
  echo "Cannot find video data!"
  exit 2
fi

# Copy video data into /dev/sdb3 (from 50M to ~140M).
dd if=video/audio.dat of=/dev/sdb3 seek=1 obs=512
dd if=video/badapple.dat of=/dev/sdb3 seek=102400 obs=512


