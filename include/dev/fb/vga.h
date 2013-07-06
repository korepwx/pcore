// @file: include/dev/fb/vga.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-06
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_DEV_FB_VGA_H_517266CCE5E411E2AAF874E50BEE6214
#define _INCLUDE_DEV_FB_VGA_H_517266CCE5E411E2AAF874E50BEE6214
#pragma once

#include <dev/fb/fb.h>

/**
 * @brief Call probe on all VGA devices and initialize existing ones.
 * @return Zero if success.
 */
int vga_probe_all();

#endif // _INCLUDE_DEV_FB_VGA_H_517266CCE5E411E2AAF874E50BEE6214
