// @file: kern/init/boot_cons.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-11
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stdio.h>
#include <string.h>
#include <dev/fb/fb.h>
#include <pcore/stdio.h>
#include <pcore/boot_cons.h>

// ---- Boot console context ----
struct _BootConsole {
  VideoAdapter *va;
  int linesize;             // exact video adapter line size.
  int bufsize;              // exact video adapter buffer size.
  int cursor;               // cursor position.
  uint16_t *buf;            // buffer size (each composed of a char and a mode).
  uint16_t mode;            // current text mode code.
};
typedef struct _BootConsole BootConsole;

// ---- putchar for boot console ----
static BootConsole boot_cons;

static int boot_cons_putchar(int ch) 
{
  VideoAdapter *va = boot_cons.va;
  ch = (ch & 0xff) | boot_cons.mode;
  
  switch (ch & 0xff) {
    case '\r':
      boot_cons.cursor -= boot_cons.cursor % boot_cons.linesize;
      break;
    case '\n':
      boot_cons.cursor += 
        boot_cons.linesize - boot_cons.cursor % boot_cons.linesize;
      break;
    default:
      boot_cons.buf[boot_cons.cursor++] = ch;
      break;
  }
  
  // If cursor position exceeds screen, we should roll lines.
  if (boot_cons.cursor >= boot_cons.bufsize) {
    memmove(boot_cons.buf,
            boot_cons.buf + boot_cons.linesize,
            (boot_cons.bufsize - boot_cons.linesize) * sizeof(uint16_t));
    uint16_t *pend = boot_cons.buf + boot_cons.bufsize;
    uint16_t *p = pend - boot_cons.linesize;
    for (; p < pend; ++p) {
      *p = ' ' | boot_cons.mode;
    }
    boot_cons.cursor -= boot_cons.linesize;
  }
  
  // update the hardware cursor.
  va_set_cursor(boot_cons.va, boot_cons.cursor);
  return 1;
}

// ---- Clear for boot console ----
// note that clear text mode output with 0x0 will let cursor disappear.
int boot_cons_clear()
{
  boot_cons.cursor = 0;
  boot_cons.mode = 0x0700;  // black on white
  int i;
  for (i=0; i<boot_cons.bufsize; ++i) {
    boot_cons.buf[i] = ' ' | boot_cons.mode;
  }
  va_set_cursor(boot_cons.va, boot_cons.cursor);
  return 0;
}

// ---- init the boot console ----
int boot_cons_init()
{
  // Initialize VGA.
  VideoAdapter* va = va_first();
  va_switch_mode(va, VM_VGA_C80x25);
  // Initialize boot console context.
  boot_cons.va = va;
  boot_cons.linesize = va->va_info->vi_width;
  boot_cons.bufsize = va->va_info->vi_height * va->va_info->vi_width;
  boot_cons.buf = (uint16_t*)va->va_buffer;
  // Register put char.
  kset_putchar(boot_cons_putchar);
  // clear boot console.
  boot_cons_clear();
  // return success.
  return 0;
}
