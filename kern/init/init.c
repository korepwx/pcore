// @file: kern/init/init.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-03
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stddef.h>
#include <string.h>
#include <asm/io.h>
#include <dev/fb/fb.h>
#include <dev/fb/vga.h>

static void vga_text() {
  VideoAdapter *va = va_first();
  if (va == NULL)
    return;
  
  va_switch_mode(va, VM_VGA_C80x25);
  va_set_palette(va, &kDefault16ConsPalette);
  memset(va->va_buffer, 0, va->va_buffer_size);
  
  int i;
  for (i=0; i<16; ++i) {
    va->va_buffer[i*2] = ' ';
    va->va_buffer[i*2+1] = (i) << 4;
  }
}

static void vga_320_200() {
  VideoAdapter *va = va_first();
  if (va == NULL)
    return;
  va_switch_mode(va, VM_VGA_CG320);
  va_set_palette(va, &kDefault256GrayPalette);
  
  uint8_t *p = va->va_buffer;
  int i;
  for (i=0; i<va->va_info->vi_height; ++i) {
    int j;
    for (j=0; j<va->va_info->vi_width; ++j) {
      *(++p) = j & 0xff;
    }
  }
}

static void vga_640_480() {
  VideoAdapter *va = va_first();
  if (va != 0) {
    va_switch_mode(va, VM_VGA_BG640);
    memset(va->va_buffer, 0x1, va->va_buffer_size);
  }
}

/**
 * @brief The entry point for pCore.
 * kern_init is called by kern_entry. @sa arch/<platform-depended>/init/entry.S
 */
int kern_init()
{
  va_init();
  vga_probe_all();
  vga_text();
  //vga_640_480();
  //vga_320_200();
  while (1) ;
  
  return 0;
}

