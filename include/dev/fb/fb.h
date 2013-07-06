// @file: include/dev/fb/fb.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-06
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_DEV_FB_FB_H_7A116C74E5FB11E2B7D2AD7191355202
#define _INCLUDE_DEV_FB_FB_H_7A116C74E5FB11E2B7D2AD7191355202
#pragma once

#include <stddef.h>
#include <color.h>
#include <pcore/list.h>

// ---- The following are video device and video mode interfaces ----

/**
 * @brief Video mode info.
 */
struct _VideoInfo {
  int vi_mode;
// ---- Supported video mode numbers ----
#define VM_VGA_C80x25   0       // vga 80x25 16 colors text
#define VM_VGA11        1       // vga 640x480 2 colors
#define VM_VGA_BG640    1
#define VM_VGA13        2       // vga 320x200 256 colors
#define VM_VGA_CG320    2
  int vi_flags;
#define V_INFO_COLOR      (1 << 0)
#define V_INFO_FONT       (1 << 1)
#define V_INFO_PALLETE    (1 << 2)
#define V_INFO_GRAPHICS   (1 << 3)
#define V_INFO_LINEAR     (1 << 4)
#define V_INFO_VESA       (1 << 5)
#define V_INFO_NONVGA     (1 << 6)
  int vi_width;
  int vi_height;
  int vi_cwidth;              // Character width.
  int vi_cheight;             // Character height.
  int vi_line_width;          // Line width (may be different from vi_width).
  int vi_scan_line;           // Line size in bytes. 
      // vi_scan_line * 8 / vi_line_width should be unit size in bits.
  int vi_depth;               // Color depth. (2^n colors in palette).
  int vi_planes;
  uintptr_t vi_buffer;        // Buffer base (physical address).
  uintptr_t vi_buffer_size;   // Buffer size (physical address).
  int vi_mem_model;
#define V_INFO_MM_OTHER   (-1)
#define V_INFO_MM_TEXT    0
#define V_INFO_MM_PLANAR  1   // Color values according to pallete.
#define V_INFO_MM_PACKED  2   // Packed direct color values.
#define V_INFO_MM_DIRECT  3   // Direct color values.
  // For MM_PACKED and MM_DIRECT only.
  int vi_pixel_size;
};
typedef struct _VideoInfo VideoInfo;

// Forward declaretion of video adapter.
struct _VideoAdapter;
typedef struct _VideoAdapter VideoAdapter;

/**
 * @brief Video adapter operation methods.
 */
struct _VideoAdapterOpt {
  /**
   * @brief Switch video adapter to specified mode.
   * @return Zero if success.
   */
  int (*switch_mode)(VideoAdapter *va, int va_mode);
  
  /**
   * @brief Set video adapter active palette.
   * @return Zero if success.
   */
  int (*set_palette)(VideoAdapter *va, ColorPalette *pal);
  
  // Remaining methods need to be implemented.
};
typedef struct _VideoAdapterOpt VideoAdapterOpt;

/**
 * @brief Video adapter instance.
 */
struct _VideoAdapter {
  int va_type;
#define V_DEV_OTHER   0     // Unknown
#define V_DEV_MONO    1     // Monochrome adapter
#define V_DEV_CGA     2     // Color graphics adapter
#define V_DEV_EGA     3     // Enhanced graphics adapter
#define V_DEV_VGA     4     // Video graphics adapter
  char *va_name;
  int va_flags;
#define V_ADP_MODECHANGE (1 << 1)
#define V_ADP_VESA  (1 << 4)
#define V_ADP_BOOTDISPLAY (1 << 5)
#define V_ADP_PROBED  (1 << 6)
#define V_ADP_INITIALIZED (1 << 7)
#define V_ADP_REGISTERED (1 << 8)
#define V_ADP_ATTACHED  (1 << 9)
  uint8_t *va_buffer;      // Virtual address, set after switch mode.
  size_t va_buffer_size;
  VideoInfo *va_info;
  // The video operation methods.
  VideoAdapterOpt *va_opt;
  // Chain all video adapters in a list.
  KListEntry va_list;
};

// ---- The following are operations of video adapter ----

/**
 * @brief Initialize the video adapter system.
 * @return Zero if success.
 */
extern int va_init();

/// @brief Get the first video adapter, but not lock on list.
extern VideoAdapter *va_first();

/// @brief Open video adapter list for enumerate, and return the first item.
extern VideoAdapter *va_list_open();

/// @brief Get the next video adapter in list.
extern VideoAdapter *va_list_next(VideoAdapter *va);

/// @brief Close video adapter list.
extern void va_list_close();

/// @brief Register a video adapter into list.
extern int va_reg_adapter(VideoAdapter *va);

// ---- Wrappers for va operations ----
#define va_switch_mode(va, mode)  (va)->va_opt->switch_mode((va), (mode))
#define va_set_palette(va, pal)   (va)->va_opt->set_palette((va), (pal))

#endif // _INCLUDE_DEV_FB_FB_H_7A116C74E5FB11E2B7D2AD7191355202
