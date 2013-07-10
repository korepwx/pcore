// @file: include/color.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-06
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_COLOR_H_9AD98442E62C11E2AF0D74E50BEE6214
#define _INCLUDE_COLOR_H_9AD98442E62C11E2AF0D74E50BEE6214
#pragma once

#include <stddef.h>

// ---- The following are common color types ----

/// @brief RGBA color instance.
struct _RgbaColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;    // Since struct occupy 4 bytes, I leave it here.
};
typedef struct _RgbaColor RgbaColor;

/**
 * @brief Common color palette abstract interface.
 * Memory allocation and destruction should be taken care of by the user.
 */
struct _ColorPalette {
  /// The number of colors this palette contains.
  size_t size;
  /// Number of colors including unspecified ones.
  size_t fillsize;
  /// All color data.
  RgbaColor *data;
};
typedef struct _ColorPalette ColorPalette;

// ---- The following are pre-defined color palettes ----
extern ColorPalette kDefault256GrayPalette;
extern ColorPalette kDefault256RgbPalette;
extern ColorPalette kDefault16ConsPalette;
extern ColorPalette kDefault16GrayPalette;

// ---- The following are color utilities ----

/// @brief Fit RGB color to default 256 gray palette.
static inline int k_palette_fit_to_256_gray(uint8_t r, uint8_t g, uint8_t b) {
  // accurate formula should be: Y = 0.299 * R + 0.587 * G + 0.114 * B
  // http://software.intel.com/sites/products/documentation/hpc/ipp/ippi/
  //    ippi_ch6/functn_RGBToGray.html#fig6-18
  // however, I choose a more faster way to perform the conversion.
  // (though much less accurate).
  return (r == g && g == b) ? (r) : (int)((r >> 2) + (g >> 1) + (b >> 2));
}

/// @brief Fit RGB color to default 256 rgb palette.
static inline int k_palette_fit_to_256_rgb(uint8_t r, uint8_t g, uint8_t b) {
  // This palette is ((40 * R)+(5 * G)+B) + 8 "grays" (7 grays + #000).
  if (r == g && g == b) {
    return (r <= 32) ? (0) : ((r >> 5) + 239);
  }
  
  int ri = r / 43;
  int gi = g >> 5;
  int bi = b / 52;
  
  // TODO: check this formula?
  return (ri * 40 + gi * 5 + bi);
}

#endif // _INCLUDE_COLOR_H_9AD98442E62C11E2AF0D74E50BEE6214
