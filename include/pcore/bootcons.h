// @file: include/pcore/bootcons.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-11
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_BOOTCONS_H_9163A3F0E9F311E2B2CA74E50BEE6214
#define _INCLUDE_PCORE_BOOTCONS_H_9163A3F0E9F311E2B2CA74E50BEE6214
#pragma once

#if defined(__KERNEL__)

/**
 * @brief This file provides a basic console on early boot-up.
 * After ttys has been setup, you should use tty console instead of this.
 */

/// @brief Initialize the boot-up console.
int bootcons_init();

/// @brief Clear boot-up console output and reset the cursor.
int bootcons_clear();

uint8_t bootcons_get_mode();
void bootcons_set_mode(uint8_t mode);
void bootcons_reset_mode();

#endif  // __KERNEL__

#endif // _INCLUDE_PCORE_BOOTCONS_H_9163A3F0E9F311E2B2CA74E50BEE6214
