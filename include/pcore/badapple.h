// @file: include/pcore/badapple.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-18
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_BADAPPLE_H_50EEA1C0EFB711E2854D74E50BEE6214
#define _INCLUDE_PCORE_BADAPPLE_H_50EEA1C0EFB711E2854D74E50BEE6214
#pragma once

#include <stddef.h>

/// @brief The decode daemon runs in kern_init.
void badapple_main(void);

/// @brief The scheduled callback to show next frame.
void badapple_sched(void);

#endif // _INCLUDE_PCORE_BADAPPLE_H_50EEA1C0EFB711E2854D74E50BEE6214
