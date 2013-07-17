// @file: kern/init/bootcons.c
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
#include <pcore/bootcons.h>
#include <pcore/serial.h>
#include <pcore/sync.h>

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
static BootConsole bootcons;

static void cons_putc(int ch)
{
  ch = (ch & 0xff) | bootcons.mode;
  
  switch (ch & 0xff) {
    case '\r':
      bootcons.cursor -= bootcons.cursor % bootcons.linesize;
      break;
    case '\n':
      bootcons.cursor += 
        bootcons.linesize - bootcons.cursor % bootcons.linesize;
      break;
    case '\t':
      bootcons.cursor += 
        8 - (bootcons.cursor & 0x7);
      break;
    default:
      bootcons.buf[bootcons.cursor++] = ch;
      break;
  }
  
  // If cursor position exceeds screen, we should roll lines.
  if (bootcons.cursor >= bootcons.bufsize) {
    memmove(bootcons.buf,
            bootcons.buf + bootcons.linesize,
            (bootcons.bufsize - bootcons.linesize) * sizeof(uint16_t));
    uint16_t *pend = bootcons.buf + bootcons.bufsize;
    uint16_t *p = pend - bootcons.linesize;
    for (; p < pend; ++p) {
      *p = ' ' | bootcons.mode;
    }
    bootcons.cursor -= bootcons.linesize;
  }
  
  // update the hardware cursor.
  va_set_cursor(bootcons.va, bootcons.cursor);
}

static int bootcons_putchar(int ch) 
{
  bool intr_flag;
  local_intr_save(intr_flag); 
  {
    cons_putc(ch);
    serial_putchar(ch);  // put char into serial
  }
  local_intr_restore(intr_flag);
  
  return 1;
}

// ---- Clear for boot console ----
// note that clear text mode output with 0x0 will let cursor disappear.
int bootcons_clear()
{
  bootcons.cursor = 0;
  bootcons.mode = 0x0700;  // black on white
  int i;
  for (i=0; i<bootcons.bufsize; ++i) {
    bootcons.buf[i] = ' ' | bootcons.mode;
  }
  va_set_cursor(bootcons.va, bootcons.cursor);
  return 0;
}

// ---- init the boot console ----
int bootcons_init()
{
  // Initialize VGA.
  VideoAdapter* va = va_first();
  va_switch_mode(va, VM_VGA_C80x25);
  // Initialize boot console context.
  bootcons.va = va;
  bootcons.linesize = va->va_info->vi_width;
  bootcons.bufsize = va->va_info->vi_height * va->va_info->vi_width;
  bootcons.buf = (uint16_t*)va->va_buffer;
  // Register put char.
  kset_putchar(bootcons_putchar);
  // clear boot console.
  bootcons_clear();
  // return success.
  return 0;
}

// ---- Get & Set char mode ----
void bootcons_set_mode(uint8_t mode) {
  bootcons.mode = (uint16_t)mode << 8;
}

void bootcons_reset_mode() {
  bootcons.mode = 0x0700;
}

uint8_t bootcons_get_mode() {
  return (uint8_t)((bootcons.mode >> 8) & 0xFF);
}
