// @file: include/pcore/diskopt.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-19
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_DISKOPT_H_CC0AC2F0F04611E2BB5B74E50BEE6214
#define _INCLUDE_PCORE_DISKOPT_H_CC0AC2F0F04611E2BB5B74E50BEE6214
#pragma once

// ---- This file intends to provide a basic disk IO api ----
// The basic disk IO api is only used at early stage.
#include <stddef.h>

/**
 * @brief Read @size bytes of disk data starting from @offset.
 * @return Zero if success.
 */
int kdisk_read(uint16_t ideno, void* buffer, size_t size, size_t offset);

/**
 * @brief Read [@off, @off+@n) sectors from disk.
 * @return Zero if success.
 */
int kdisk_read_secs(uint16_t ideno, void* buffer, size_t n, size_t off);

#endif // _INCLUDE_PCORE_DISKOPT_H_CC0AC2F0F04611E2BB5B74E50BEE6214
