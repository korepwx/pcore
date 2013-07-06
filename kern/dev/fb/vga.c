// @file: kern/dev/fb/vga.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-06
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <dev/fb/vga.h>
#include <string.h>
#include <errno.h>
#include <asm/io.h>
#include <pcore/memlayout.h>

// ---- VGA address & size contants -----
#define EOT                 (-1)
#define CGA_BUF_BASE        0xb8000
#define CGA_BUF_SIZE        0x08000
#define GRAPHICS_BUF_BASE   0xa0000
#define GRAPHICS_BUF_SIZE   0x10000

// ---- VGA video modes ----
// TODO: complete and check these video infos.
static VideoInfo vga_video_info[] = {
  /* VGA */
  [VM_VGA_C80x25] {
    .vi_mode = VM_VGA_C80x25,
    .vi_flags = V_INFO_COLOR | V_INFO_FONT | V_INFO_PALLETE, 
    .vi_width = 80, 
    .vi_height = 25, 
    .vi_cwidth = 8, 
    .vi_cheight = 16, 
    .vi_line_width = 80,
    .vi_scan_line = 160,
    .vi_depth = 4, 
    .vi_planes = 1,
    .vi_buffer = CGA_BUF_BASE, 
    .vi_buffer_size = CGA_BUF_SIZE, 
    .vi_mem_model = V_INFO_MM_TEXT
  },
  [VM_VGA_BG640] {
    .vi_mode = VM_VGA_BG640, 
    .vi_flags = V_INFO_GRAPHICS, 
    .vi_width = 640, 
    .vi_height = 480, 
    .vi_cwidth = 8, 
    .vi_cheight = 16, 
    .vi_line_width = 640,
    .vi_scan_line = 80,
    .vi_depth = 1, 
    .vi_planes = 1,
    .vi_buffer = GRAPHICS_BUF_BASE, 
    .vi_buffer_size = GRAPHICS_BUF_SIZE, 
    .vi_mem_model = V_INFO_MM_PLANAR
  },
  [VM_VGA_CG320] {
    .vi_mode = VM_VGA_CG320, 
    .vi_flags = V_INFO_COLOR | V_INFO_GRAPHICS | V_INFO_PALLETE, 
    .vi_width = 320, 
    .vi_height = 200, 
    .vi_cwidth = 8, 
    .vi_cheight = 8, 
    .vi_line_width = 320,
    .vi_scan_line = 320,
    .vi_depth = 8, 
    .vi_planes = 1,
    .vi_buffer = GRAPHICS_BUF_BASE, 
    .vi_buffer_size = GRAPHICS_BUF_SIZE, 
    .vi_mem_model = V_INFO_MM_PLANAR
  },
  { EOT }
};

// ---- VGA adapter operation interfaces ----
int vga_opt_switch_mode(VideoAdapter *va, int va_mode);
int vga_opt_set_palette(VideoAdapter *va, ColorPalette *pal);

static VideoAdapterOpt vga_opt = {
  .switch_mode = vga_opt_switch_mode,
  .set_palette = vga_opt_set_palette
};

// ---- VGA adapters ----
// NOTE: I now simply assume that there is only a VGA device and is enabled.
//       Please complete video adapter probes.
static VideoAdapter vga_video_adapters[] = {
  {
    .va_type = V_DEV_VGA, 
    .va_name = "vga", 
    .va_flags = V_ADP_MODECHANGE | V_ADP_BOOTDISPLAY, 
    .va_buffer = 0, 
    .va_buffer_size = 0, 
    .va_info = NULL, 
    .va_opt = &vga_opt
  },
  { EOT },
};

int vga_probe_all() {
  int ret = 0;
  VideoAdapter *va = &(vga_video_adapters[0]);
  for (; va->va_type != EOT; ++va) {
    ret = vga_opt_probe(va);
    if (ret != 0)
      ret = -1;  // TODO: more detailed errno
  }
  return 0;
}

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

// ---- VGA adapter operation imhplements ----
int vga_opt_probe(VideoAdapter *va) {
  int ret;
  if (va->va_type != V_DEV_VGA) 
    return EINVAL;
  if ((ret = vga_opt_switch_mode(va, VM_VGA_C80x25)) == 0) {
    ksetbit(va->va_flags, V_ADP_INITIALIZED | V_ADP_PROBED | V_ADP_ATTACHED);
    va_reg_adapter(va);
  }
  return ret;
}

int vga_opt_switch_mode(VideoAdapter* va, int va_mode)
{
  uint8_t *regs;
  VideoInfo *vi;
  
  if (va->va_type != V_DEV_VGA)
    return EINVAL;
  
  if (!kissetbit(va->va_flags, V_ADP_MODECHANGE))
    return EVMOPT;
  
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
    default:
      return EVMOPT;
  }
  
  vi = &(vga_video_info[va_mode]);
  write_regs(regs);
  va->va_info = vi;
  va->va_buffer = (uint8_t*)KVADDR(vi->vi_buffer);
  va->va_buffer_size = vi->vi_buffer_size;
  
  return 0;
}

static inline int vga_opt_set_palette_sub(VideoAdapter* va, ColorPalette *pal) {
  // Strange behaviour for 16-color palette in text mode:
  //  http://forum.osdev.org/viewtopic.php?p=192800#p192800
  int remap[0x40] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 
    0x05, 0x14, 0x07, 0x38, 0x39, 
    0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 
    0x3F
  };
  
  int i;
  RgbaColor *c = pal->data;
  
  for(i=0; i<pal->fillsize; ++c, ++i)
  {
    outb_p(0x03c8, remap[i]);
    outb_p(0x03c9, (c->r >> 2) & 0x3f);
    outb_p(0x03c9, (c->g >> 2) & 0x3f);
    outb_p(0x03c9, (c->b >> 2) & 0x3f);
  }
}

int vga_opt_set_palette(VideoAdapter* va, ColorPalette* pal)
{
  if (va->va_type != V_DEV_VGA)
    return EINVAL;
  if (!kissetbit(va->va_info->vi_flags, V_INFO_PALLETE))
    return EVMOPT;
  if (pal->fillsize != (1 << va->va_info->vi_depth))
    return EVMOPT;
  
  // TODO: use spinlock to protect critical section.
  if (va->va_info->vi_mode == VM_VGA_C80x25
      /* && va->va_info->vi_depth == 4 */) {
    return vga_opt_set_palette_sub(va, pal);
  }
  
  RgbaColor *c = pal->data, *cend = pal->data + pal->fillsize;
  outb_p(0x03c8, 0);
  for(; c < cend; ++c)
  {
    outb_p(0x03c9, (c->r >> 2) & 0x3f);
    outb_p(0x03c9, (c->g >> 2) & 0x3f);
    outb_p(0x03c9, (c->b >> 2) & 0x3f);
  }
  
  return 0;
}
