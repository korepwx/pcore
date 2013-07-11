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
#include <errno.h>
#include <pcore/list.h>

// ---- Forward declaretions ----
struct _VideoInfo;
typedef struct _VideoInfo VideoInfo;
struct _VideoAdapter;
typedef struct _VideoAdapter VideoAdapter;
struct _VideoAdapterOpt;
typedef struct _VideoAdapterOpt VideoAdapterOpt;

// ---- The following are video device and video mode interfaces ----

/**
 * @brief Video adapter operation methods.
 * 
 * Operations may different between modes.  So this struct should be carried 
 * in video mode, not video adapter.
 */
struct _VideoAdapterOpt {
  /**
   * @brief Switch video adapter to specified mode.
   * @return Zero if success.
   */
  int (*switch_mode)(VideoAdapter *va, int va_mode);
  
  /**
   * @brief Clear the video adapter with palette color 0, or #000.
   */
  int (*clear_output)(VideoAdapter *va);
  
  /**
   * @brief Set video adapter active palette.
   * @return Zero if success.
   */
  int (*set_palette)(VideoAdapter *va, ColorPalette *pal);
  
  /**
   * @brief Set mask of planes.
   * @return Zero if success.
   */
  int (*set_plane_mask)(VideoAdapter *va, uint8_t mask);
  
  /**
   * @brief Get mask of planes.
   * @return Zero if success.
   */
  int (*get_plane_mask)(VideoAdapter *va, uint8_t *mask);
  
  /**
   * @brief Set the hardware cursor of video mode.
   * @param pos: The position of cursor (pos = y * width + x).
   * @return Zero if success.
   */
  int (*set_cursor)(VideoAdapter *va, uint16_t pos);
  
  // ---- The following are framebuffer IO routines ----
  
  // These routines does not provide a "unique" interface for different video 
  // adapters to input/output exactly same video mode data.  They require, 
  // at least, the input/output data match the video mode.  However, such 
  // differences should be covered as the 640x480x16 VGA IO requires the caller 
  // to switch among 4 planes.  These routines provide an interface to regard 
  // the video data as normal N-bit linear, if the color depth is N.
  
  /**
   * @brief Video framebuffer IO read.
   * @param data: Input/output bytes. Should be N-bit linear video data, 
   *  and the resolution should match (x2 - x1) * (y2 - y1).  Bits should 
   *  be placed from left to right in geometry as high to low in memory. 
   *  Multiple bits which represent one pixel should also be placed from 
   *  high memory to low.
   * @param x1: Start offset of columns, must be multiples of 8.
   * @param y1: Start offset of rows.
   * @param x2: End offset of columns, must be multiples of 8 (excluded).
   * @param y2: End offset of rows (excluded).
   * @return Zero if success.
   * @note: Neither the validation of 8 multiple bounds nor the offset range 
   *  would be performed.  The caller must ensure the arguments, otherwise 
   *  unspecified behaviour might happen.
   */
  int (*video_read)
    (VideoAdapter *va, uint8_t *data, int x1, int y1, int x2, int y2);
  
  /// @brief Video framebuffer IO write.
  int (*video_write)
    (VideoAdapter *va, uint8_t *data, int x1, int y1, int x2, int y2);
  
  // TODO: consider provide a common text framebuffer IO.
  // Remaining methods need to be implemented.
};

/**
 * @brief Video mode info.
 */
struct _VideoInfo {
  int vi_mode;
// ---- Supported video mode numbers ----
#define VM_VGA_C80x25   0       // vga 80x25 16 colors text
#define VM_VGA11        1       // vga 640x480 2 colors
#define VM_VGA_BG640    1
#define VM_VGA12        2       // vga 640x480 16 colors
#define VM_VGA_CG640    2
#define VM_VGA13        3       // vga 320x200 256 colors
#define VM_VGA_CG320    3
  int vi_flags;
#define V_INFO_COLOR      (1 << 0)
#define V_INFO_FONT       (1 << 1)
#define V_INFO_PALLETE    (1 << 2)
#define V_INFO_GRAPHICS   (1 << 3)
#define V_INFO_LINEAR     (1 << 4)    // Planes need not to be selected.
#define V_INFO_VESA       (1 << 5)
#define V_INFO_NONVGA     (1 << 6)
  int vi_width;
  int vi_height;
  int vi_cwidth;              // Character width.
  int vi_cheight;             // Character height.
  int vi_line_width;          // Line width (may be different from vi_width).
  int vi_scan_line;           // Line size in bytes. 
                              // Multiple planes only count once.
  int vi_depth;               // Color depth. (2^n colors in palette).
      // Color depth must either be multiples of 8 (like 24), 
      // or 2^k (like 1, 2, 4, 8).
  int vi_planes;              // Planes this video mode uses.
      // Planes may be 1, 2 or 4.
  uintptr_t vi_buffer;        // Buffer base (physical address).
  size_t vi_buffer_size;      // Buffer size
  int vi_mem_model;
#define V_INFO_MM_OTHER   (-1)
#define V_INFO_MM_TEXT    0
#define V_INFO_MM_PLANAR  1   // Color values masked by planes.
#define V_INFO_MM_PACKED  2   // Packed direct color values.
#define V_INFO_MM_DIRECT  3   // Direct color values.
#define V_INFO_MM_CGA     4   // Color values encoded by color palettes.
  // For MM_PACKED and MM_DIRECT only.
  int vi_pixel_size;
  // The video operation methods.
  VideoAdapterOpt vi_adp_opt;
};

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
#define PCORE_VA_CALL(METHOD, PARAMS)  \
  ((va->va_info->vi_adp_opt. METHOD ) != NULL ? \
    ((va->va_info->vi_adp_opt. METHOD ) PARAMS) : \
    EVMOPT \
  )

static inline int va_switch_mode(VideoAdapter* va, int va_mode) {
  return PCORE_VA_CALL(switch_mode, (va, va_mode));
}
static inline int va_clear_output(VideoAdapter* va) {
  return PCORE_VA_CALL(clear_output, (va));
}
static inline int va_set_palette(VideoAdapter* va, ColorPalette* pal) {
  return PCORE_VA_CALL(set_palette, (va, pal));
}
static inline int va_set_plane_mask(VideoAdapter* va, uint8_t mask) {
  return PCORE_VA_CALL(set_plane_mask, (va, mask));
}
static inline int va_get_plane_mask(VideoAdapter* va, uint8_t *mask) {
  return PCORE_VA_CALL(get_plane_mask, (va, mask));
}
static inline int va_set_cursor(VideoAdapter* va, uint16_t pos) {
  return PCORE_VA_CALL(set_cursor, (va, pos));
}
static inline int va_video_read
  (VideoAdapter* va, uint8_t *data, int x1, int y1, int x2, int y2)
{
  return PCORE_VA_CALL(video_read, (va, data, x1, y1, x2, y2));
}
static inline int va_video_write
  (VideoAdapter* va, uint8_t *data, int x1, int y1, int x2, int y2)
{
  return PCORE_VA_CALL(video_write, (va, data, x1, y1, x2, y2));
}

#endif // _INCLUDE_DEV_FB_FB_H_7A116C74E5FB11E2B7D2AD7191355202
