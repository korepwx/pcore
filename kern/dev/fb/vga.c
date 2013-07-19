// @file: kern/dev/fb/vga.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-06
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <dev/fb/fb.h>
#include <dev/fb/vga.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <asm/io.h>
#include <pcore/pmm.h>

// Korepwx: special hack; when VGA is setup, pmm has not been initialized.
#undef KADDR
#define KADDR(X)  (X + KERNBASE)

// ---- VGA address & size contants -----
#define EOT                 (-1)
#define CGA_BUF_BASE        0xb8000
#define CGA_BUF_SIZE        0x08000
#define GRAPHICS_BUF_BASE   0xa0000
#define GRAPHICS_BUF_SIZE   0x10000

// ---- VGA adapter operation interfaces ----
int vga_opt_probe               (VideoAdapter *va);
int vga_opt_switch_mode         (VideoAdapter *va, int va_mode);
int vga_opt_clear_output        (VideoAdapter *va);
int vga_opt_set_palette_16color (VideoAdapter *va, ColorPalette *pal);
int vga_opt_set_palette_256color(VideoAdapter *va, ColorPalette *pal);
int vga_opt_set_plane_mask      (VideoAdapter *va, uint8_t mask);
int vga_opt_get_plane_mask      (VideoAdapter *va, uint8_t *mask);
int vga_opt_set_cursor          (VideoAdapter *va, uint16_t pos);

int vga_opt_video_read_linear
  (VideoAdapter *va, uint8_t *data, int x1, int y1, int x2, int y2);
int vga_opt_video_write_linear
  (VideoAdapter *va, uint8_t *data, int x1, int y1, int x2, int y2);
int vga_opt_video_read_planes
  (VideoAdapter *va, uint8_t *data, int x1, int y1, int x2, int y2);
int vga_opt_video_write_planes
  (VideoAdapter *va, uint8_t *data, int x1, int y1, int x2, int y2);

// ---- VGA video modes ----
// TODO: complete and check these video infos.
static VideoInfo vga_video_info[] = {
  /* VGA */
  [VM_VGA_C80x25] {
    .vi_mode = VM_VGA_C80x25,
    .vi_flags = V_INFO_COLOR | V_INFO_FONT | V_INFO_PALLETE | V_INFO_LINEAR, 
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
    .vi_mem_model = V_INFO_MM_TEXT,
    .vi_adp_opt = {
      .switch_mode = vga_opt_switch_mode,
      .clear_output = vga_opt_clear_output,
      .set_palette = vga_opt_set_palette_16color,
      .set_plane_mask = vga_opt_set_plane_mask,
      .get_plane_mask = vga_opt_get_plane_mask,
      .set_cursor = vga_opt_set_cursor
    }
  },
  [VM_VGA_BG640] {
    .vi_mode = VM_VGA_BG640, 
    .vi_flags = V_INFO_GRAPHICS | V_INFO_LINEAR, 
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
    .vi_mem_model = V_INFO_MM_CGA,
    .vi_adp_opt = {
      .switch_mode = vga_opt_switch_mode,
      .clear_output = vga_opt_clear_output,
      .set_palette = NULL,
      .set_plane_mask = vga_opt_set_plane_mask,
      .get_plane_mask = vga_opt_get_plane_mask,
      .video_read = vga_opt_video_read_linear,
      .video_write = vga_opt_video_write_linear
    }
  },
  [VM_VGA_CG640] {
    .vi_mode = VM_VGA_CG640,
    .vi_flags = V_INFO_COLOR | V_INFO_GRAPHICS | V_INFO_PALLETE,
    .vi_width = 640,
    .vi_height = 480,
    .vi_cwidth = 8,
    .vi_cheight = 16,
    .vi_line_width = 640,
    .vi_scan_line = 80,
    .vi_depth = 4,
    .vi_planes = 4,
    .vi_buffer = GRAPHICS_BUF_BASE,
    .vi_buffer_size = GRAPHICS_BUF_SIZE,
    .vi_mem_model = V_INFO_MM_PLANAR,
    .vi_adp_opt = {
      .switch_mode = vga_opt_switch_mode,
      .clear_output = vga_opt_clear_output,
      .set_palette = vga_opt_set_palette_16color,
      .set_plane_mask = vga_opt_set_plane_mask,
      .get_plane_mask = vga_opt_get_plane_mask,
      .video_read = vga_opt_video_read_planes,
      .video_write = vga_opt_video_write_planes
    }
  },
  [VM_VGA_CG320] {
    .vi_mode = VM_VGA_CG320, 
    .vi_flags = V_INFO_COLOR | V_INFO_GRAPHICS | V_INFO_PALLETE | V_INFO_LINEAR, 
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
    .vi_mem_model = V_INFO_MM_CGA,
    .vi_adp_opt = {
      .switch_mode = vga_opt_switch_mode,
      .clear_output = vga_opt_clear_output,
      .set_palette = vga_opt_set_palette_256color,
      .set_plane_mask = vga_opt_set_plane_mask,
      .get_plane_mask = vga_opt_get_plane_mask,
      .video_read = vga_opt_video_read_linear,
      .video_write = vga_opt_video_write_linear
    }
  },
  { EOT }
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
    .va_info = NULL
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

// ---- VGA adapter operation imhplements ----

int vga_opt_probe(VideoAdapter *va) {
  int ret;
  kassert(va->va_type == V_DEV_VGA);
  
  if ((ret = vga_opt_switch_mode(va, VM_VGA_C80x25)) == 0) {
    ksetmask(va->va_flags, V_ADP_INITIALIZED | V_ADP_PROBED | V_ADP_ATTACHED);
    va_reg_adapter(va);
  }
  return ret;
}

int vga_opt_switch_mode(VideoAdapter* va, int va_mode)
{
  VideoInfo *vi;
  int ret;
  
  kassert(va->va_type == V_DEV_VGA);
  kassert(kissetmask(va->va_flags, V_ADP_MODECHANGE));
  
  extern int vga_opt_switch_mode_sub(VideoAdapter* va, int va_mode);
  if ((ret = vga_opt_switch_mode_sub(va, va_mode)) != 0) 
    return ret;
  
  vi = &(vga_video_info[va_mode]);
  va->va_info = vi;
  va->va_buffer = (uint8_t*)KADDR(vi->vi_buffer);
  va->va_buffer_size = vi->vi_buffer_size;
  
  return 0;
}

int vga_opt_clear_output(VideoAdapter *va)
{
  int i;
  uint8_t origin_plane;
  
  if (va->va_info->vi_planes > 1) {
    if ((i = vga_opt_get_plane_mask(va, &origin_plane)) != 0)
      return i;
    for (i=0; i<va->va_info->vi_planes; ++i) {
      vga_opt_set_plane_mask(va, 1 << i);
      memset(va->va_buffer, 0, va->va_buffer_size);
    }
    if ((i = vga_opt_set_plane_mask(va, origin_plane)) != 0)
      return i;
  } else {
    memset(va->va_buffer, 0, va->va_buffer_size);
  }
  
  return 0;
}

// ---- Set palette routines ----

#define PCORE_CHECK_VA_PALETTE() do { \
  kassert(va->va_type == V_DEV_VGA); \
  kassert(kissetmask(va->va_info->vi_flags, V_INFO_PALLETE)); \
  kassert(pal->fillsize == (1 << va->va_info->vi_depth)); \
} while (0);

int vga_opt_set_palette_16color(VideoAdapter* va, ColorPalette *pal) {
  PCORE_CHECK_VA_PALETTE();
  
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
  
  return 0;
}

int vga_opt_set_palette_256color(VideoAdapter* va, ColorPalette* pal)
{
  PCORE_CHECK_VA_PALETTE();
  
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

int vga_opt_set_plane_mask(VideoAdapter *va, uint8_t plane_mask)
{
  outb(0x03c4, 0x2);
  outb(0x03c5, plane_mask);
  return 0;
}

int vga_opt_get_plane_mask(VideoAdapter *va, uint8_t *plane_mask)
{
  outb(0x03c4, 0x2);
  *plane_mask = inb(0x03c5);
  return 0;
}

// ---- Hardware cursor support ----
int vga_opt_set_cursor(VideoAdapter *va, uint16_t pos)
{
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));
  return 0;
}

// ---- Common video io routines ----
#define PCORE_CHECK_VIDEO_IO()  do { \
  kassert(va->va_type == V_DEV_VGA); \
  kassert(kissetmask(va->va_info->vi_flags, V_INFO_GRAPHICS)); \
} while (0);

#define PCORE_IO_MEMCPY(VABUF, DATABUF, SIZE, ISWRITE)  do { \
  if (ISWRITE) { \
    memcpy((VABUF), (DATABUF), (SIZE)); \
  } else { \
    memcpy((DATABUF), (VABUF), (SIZE)); \
  } \
} while (0);

static inline int vga_opt_video_io_linear
  (VideoAdapter *va, uint8_t *data, int x1, int y1, int x2, int y2, bool write)
{
  PCORE_CHECK_VIDEO_IO();

  // If io region occupies whole columnes.
  if (x1 == 0 && x2 == va->va_info->vi_width) {
    PCORE_IO_MEMCPY(
      va->va_buffer + va->va_info->vi_scan_line * y1, 
      data,
      (y2 - y1) * va->va_info->vi_scan_line,
      write
    );
  }
  
  // Otherwise, we must deal with columnes in each line.
  else {
    ptrdiff_t init_skip, line_size;
    
    // note: as is required, color depth must be multiples of 8,
    //       or 2^k.
    
    if (va->va_info->vi_depth >= 8) {
      init_skip = y1 * va->va_info->vi_scan_line  // skip previous lines.
        + x1 * (va->va_info->vi_depth >> 3);      // skip after columnes
      line_size 
        = (x2 - x1) * (va->va_info->vi_depth >> 3); // bytes each line copy.
    } else {
      int order;
      // estimate each unit order.
      switch (va->va_info->vi_depth) {
        case 1: order = 0; break;
        case 2: order = 1; break;
        case 4: order = 2; break;
        default: return EINVAL;
      }
      order = 3 - order;
      // get the skip settings.
      init_skip = y1 * va->va_info->vi_scan_line + (x1 >> order);
      line_size = (x2 - x1) >> order;
    }
    
    // The first unit affected. 
    uint8_t *p, *d;
    p = va->va_buffer + init_skip;
    d = data;
    
    // loop for each line.
    int i;
    for (i = 0; 
        i < y2 - y1;
        ++i, p += va->va_info->vi_scan_line, d += line_size) {
      PCORE_IO_MEMCPY(
        p, 
        d,
        line_size,
        write
      );
    }
  }
  
  return 0;
}

int vga_opt_video_read_linear
  (VideoAdapter *va, uint8_t *data, int x1, int y1, int x2, int y2)
{
  return vga_opt_video_io_linear(va, data, x1, y1, x2, y2, 0);
}
  
int vga_opt_video_write_linear
  (VideoAdapter *va, uint8_t *data, int x1, int y1, int x2, int y2)
{
  return vga_opt_video_io_linear(va, data, x1, y1, x2, y2, 1);
}

// ---- video io routines for vga12 ----
int vga_opt_video_io_vga12
  (VideoAdapter *va, uint8_t *data, int x1, int y1, int x2, int y2, bool write)
{
  PCORE_CHECK_VIDEO_IO();
  
  // Backup original plane mask.
  int ret;
  uint8_t origin_plane;
  if ((ret = vga_opt_get_plane_mask(va, &origin_plane)) != 0)
    return ret;
  
  // TODO: change following codes to meet the demands of all planar modes.
  // Detect line size and line skip.
  ptrdiff_t init_skip, line_size, line_skip;
  
  init_skip = y1 * va->va_info->vi_scan_line + (x1 >> 3);
  line_size = (x2 - x1) >> 3;
  line_skip = va->va_info->vi_scan_line - line_size;

  // The reading & writing pointer.
  uint8_t *p, *d;
  
  // copy by region.
  int m, i;
#undef GET_BIT
#define GET_BIT(V, N) (((V) >> (N)) & 0x1)
  
  if (write) {
    // loop for each plane.
    for (m = 0; m < 4; ++m) {
      vga_opt_set_plane_mask(va, 1 << m);
      // Reset pointer to the first unit.
      p = va->va_buffer + init_skip;
      d = data;
      // loop for each line.
      for (i = 0; i < y2 - y1; ++i, p += line_skip) {
        uint8_t *pend = p + ((x2 - x1) >> 3);
        // loop for each group.
        for (; p < pend; ++p, d += 4) {
          *p = (GET_BIT(*d, m+4) << 7)
              | (GET_BIT(*d, m) << 6)
              | (GET_BIT(*(d+1), m+4) << 5)
              | (GET_BIT(*(d+1), m) << 4)
              | (GET_BIT(*(d+2), m+4) << 3)
              | (GET_BIT(*(d+2), m) << 2)
              | (GET_BIT(*(d+3), m+4) << 1)
              | (GET_BIT(*(d+3), m) << 0);
        }
      }
    }
  } 
  
  else {
    // TODO: finish read!
    // clear output to zero.
    memset(data, 0, ((x2 - x1) * (y2 - y1)) >> 1);
    // loop for each plane.
    for (m = 0; m < 4; ++m) {
      vga_opt_set_plane_mask(va, 1 << m);
      // Reset pointer to the first unit.
      p = va->va_buffer + init_skip;
      d = data;
      // loop for each line.
      for (i = 0; i < y2 - y1; ++i, p += line_skip) {
        uint8_t *pend = p + ((x2 - x1) >> 3);
        // loop for each group.xx
        for (; p < pend; ++p) {
          uint8_t v = *p;
          *d++ |= (GET_BIT(v, 7) << (m + 4)) | (GET_BIT(v, 6) << (m));
          *d++ |= (GET_BIT(v, 5) << (m + 4)) | (GET_BIT(v, 4) << (m));
          *d++ |= (GET_BIT(v, 3) << (m + 4)) | (GET_BIT(v, 2) << (m));
          *d++ |= (GET_BIT(v, 1) << (m + 4)) | (GET_BIT(v, 0) << (m));
        }
      }
    }
  }
  
  // Restore original plane mask.
  if ((ret = vga_opt_set_plane_mask(va, origin_plane)) != 0)
    return ret;
  
  return 0;
}

int vga_opt_video_read_planes
  (VideoAdapter *va, uint8_t *data, int x1, int y1, int x2, int y2)
{
  return vga_opt_video_io_vga12(va, data, x1, y1, x2, y2, 0);
}
int vga_opt_video_write_planes
  (VideoAdapter *va, uint8_t *data, int x1, int y1, int x2, int y2)
{
  return vga_opt_video_io_vga12(va, data, x1, y1, x2, y2, 1);
}
