// @file: arch/i386/include/asm/reg.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-11
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _ARCH_I386_INCLUDE_ASM_REG_H_07458790EA3111E2998C74E50BEE6214
#define _ARCH_I386_INCLUDE_ASM_REG_H_07458790EA3111E2998C74E50BEE6214
#pragma once

#include <stddef.h>
#include <pcore/compiler.h>

#define PCORE_DEFINE_READ_REG(REG, TYPE, TYPEF) \
  static inline TYPE read_ ## REG (void) { \
    TYPE ret; \
    asm volatile ("mov" #TYPEF " %%" #REG ", %0" : "=r" (ret)); \
    return ret; \
  }
  
PCORE_DEFINE_READ_REG(ebp, uint32_t, l);
PCORE_DEFINE_READ_REG(esp, uint32_t, l);

#endif // _ARCH_I386_INCLUDE_ASM_REG_H_07458790EA3111E2998C74E50BEE6214
