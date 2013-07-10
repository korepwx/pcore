// @file: kern/dev/fb/vga-mode.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-08
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <asm/io.h>
#include <dev/fb/vga.h>
#include <errno.h>

// ---- VGA mode details ----
// Chris Giese <geezer@execpc.com>  http://my.execpc.com/~geezer
#define VGA_AC_INDEX    0x3C0
#define VGA_AC_WRITE    0x3C0
#define VGA_AC_READ   0x3C1
#define VGA_MISC_WRITE    0x3C2
#define VGA_SEQ_INDEX   0x3C4
#define VGA_SEQ_DATA    0x3C5
#define VGA_DAC_READ_INDEX  0x3C7
#define VGA_DAC_WRITE_INDEX 0x3C8
#define VGA_DAC_DATA    0x3C9
#define VGA_MISC_READ   0x3CC
#define VGA_GC_INDEX    0x3CE
#define VGA_GC_DATA     0x3CF
/*      COLOR emulation   MONO emulation */
#define VGA_CRTC_INDEX    0x3D4   /* 0x3B4 */
#define VGA_CRTC_DATA   0x3D5   /* 0x3B5 */
#define VGA_INSTAT_READ   0x3DA

#define VGA_NUM_SEQ_REGS  5
#define VGA_NUM_CRTC_REGS 25
#define VGA_NUM_GC_REGS   9
#define VGA_NUM_AC_REGS   21
#define VGA_NUM_REGS    (1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS + \
        VGA_NUM_GC_REGS + VGA_NUM_AC_REGS)
        
static unsigned char g_80x25_text[] =
{
/* MISC */
  0x67,
/* SEQ */
  0x03, 0x00, 0x03, 0x00, 0x02,
/* CRTC */
  0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
  0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x50,
  0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
  0xFF,
/* GC */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
  0xFF,
/* AC */
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x0C, 0x00, 0x0F, 0x08, 0x00
};

unsigned char g_640x480x2[] =
{
/* MISC */
  0xE3,
/* SEQ */
  0x03, 0x01, 0x0F, 0x00, 0x06,
/* CRTC */
  0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
  0xFF,
/* GC */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0F,
  0xFF,
/* AC */
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x01, 0x00, 0x0F, 0x00, 0x00
};

unsigned char g_640x480x16[] =
{
/* MISC */
  0xE3,
/* SEQ */
  0x03, 0x01, 0x08, 0x00, 0x06,
/* CRTC */
  0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
  0xFF,
/* GC */
  0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
  0xFF,
/* AC */
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
  0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x01, 0x00, 0x0F, 0x00, 0x00
};

static unsigned char g_320x200x256[] =
{
/* MISC */
  0x63,
/* SEQ */
  0x03, 0x01, 0x0F, 0x00, 0x0E,
/* CRTC */
  0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
  0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
  0xFF,
/* GC */
  0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
  0xFF,
/* AC */
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x41, 0x00, 0x0F, 0x00, 0x00
};

static void write_regs(unsigned char *regs)
{
  unsigned i;

  /* write MISCELLANEOUS reg */
  outb(VGA_MISC_WRITE, *regs);
  regs++;
  /* write SEQUENCER regs */
  for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
  {
    outb(VGA_SEQ_INDEX, i);
    outb(VGA_SEQ_DATA, *regs);
    regs++;
  }
  /* unlock CRTC registers */
  outb(VGA_CRTC_INDEX, 0x03);
  outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) | 0x80);
  outb(VGA_CRTC_INDEX, 0x11);
  outb(VGA_CRTC_DATA, inb(VGA_CRTC_DATA) & ~0x80);
  /* make sure they remain unlocked */
  regs[0x03] |= 0x80;
  regs[0x11] &= ~0x80;
  /* write CRTC regs */
  for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
  {
    outb(VGA_CRTC_INDEX, i);
    outb(VGA_CRTC_DATA, *regs);
    regs++;
  }
  /* write GRAPHICS CONTROLLER regs */
  for(i = 0; i < VGA_NUM_GC_REGS; i++)
  {
    outb(VGA_GC_INDEX, i);
    outb(VGA_GC_DATA, *regs);
    regs++;
  }
  /* write ATTRIBUTE CONTROLLER regs */
  for(i = 0; i < VGA_NUM_AC_REGS; i++)
  {
    (void)inb(VGA_INSTAT_READ);
    outb(VGA_AC_INDEX, i);
    outb(VGA_AC_WRITE, *regs);
    regs++;
  }
  /* lock 16-color palette and unblank display */
  (void)inb(VGA_INSTAT_READ);
  outb(VGA_AC_INDEX, 0x20);
}

int vga_opt_switch_mode_sub(VideoAdapter* va, int va_mode)
{
  uint8_t *regs;
  
  switch (va_mode) {
    case VM_VGA_C80x25:
      regs = g_80x25_text;
      break;
    case VM_VGA_BG640:
      regs = g_640x480x2;
      break;
    case VM_VGA_CG320:
      regs = g_320x200x256;
      break;
    case VM_VGA_CG640:
      regs = g_640x480x16;
      break;
    default:
      return EVMOPT;
  }
  
  write_regs(regs);
  return 0;
}