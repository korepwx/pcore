// @file: include/pcore/clock.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-17
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_CLOCK_H_4EFC188EEEC211E2A7DC53A987700B98
#define _INCLUDE_PCORE_CLOCK_H_4EFC188EEEC211E2A7DC53A987700B98
#pragma once

#include <stddef.h>

/// @brief Initialize the system clock.
void clock_init(void);

/// @brief Increase system clock by one.
void clock_inc(void);

/// @brief Reset the system clock value.
void clock_reset(void);

/// @brief Read the system clock value.
clock_t ksysclock(void);

/// @brief Ticks in one second.
#define CLOCKS_PER_SECOND   30

#endif // _INCLUDE_PCORE_CLOCK_H_4EFC188EEEC211E2A7DC53A987700B98
