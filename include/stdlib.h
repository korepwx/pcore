// @file: include/stdlib.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-18
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_STDLIB_H_213D0090EF5111E2BDCB74E50BEE6214
#define _INCLUDE_STDLIB_H_213D0090EF5111E2BDCB74E50BEE6214
#pragma once

#include <stddef.h>

// ---- Fake malloc/free ----
#include <pcore/kmalloc.h>

#if defined(__KERNEL__)
static inline void* malloc(size_t n) {
  return kmalloc(n);
}
static inline void free(void* p) {
  kfree(p);
}
#endif  // __KERNEL__

// ---- Randomize utilities ----
#define RAND_MAX    2147483647UL
int rand(void);
void srand(uint32_t seed);

#endif // _INCLUDE_STDLIB_H_213D0090EF5111E2BDCB74E50BEE6214
