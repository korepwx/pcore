// @file: test/dev/fb.test.cc
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-10
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <dev/fb/fb.h>

static inline void vga_640x480x16() 
{
  extern uint8_t test_640x480x16_img[];
  VideoAdapter *va = va_first();
  
  va_switch_mode(va, VM_VGA_CG640);
  va_set_palette(va, &kDefault16GrayPalette);
  va_clear_output(va);
  
  va_video_write(va, test_640x480x16_img, 0, 0, 640, 480);
}

/**
 * @brief Test the functionality on boot.
 */
int test_dev_fb()
{
  vga_640x480x16();
  return 0;
}
