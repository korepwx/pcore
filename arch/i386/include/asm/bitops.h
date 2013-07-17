// @file: arch/i386/include/asm/bitops.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-16
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _ARCH_I386_INCLUDE_ASM_BITOPS_H_3D497A58EDE311E28A2274E50BEE6214
#define _ARCH_I386_INCLUDE_ASM_BITOPS_H_3D497A58EDE311E28A2274E50BEE6214
#pragma once

#include <stddef.h>

// find last set bit in word
static inline size_t ufls(size_t word)
{
  asm("bsr %1,%0"
      : "=r" (word)
      : "rm" (word));
  return word + 1;
}

static inline int fls(int x)
{
  int r = 0;
  asm("bsrl %1,%0\n\t"
      "jnz 1f\n\t"
      "movl $-1,%0\n"
      "1:" : "=r" (r) : "rm" (x));
  return r + 1;
}

#endif // _ARCH_I386_INCLUDE_ASM_BITOPS_H_3D497A58EDE311E28A2274E50BEE6214
