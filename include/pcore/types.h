// @file: include/pcore/types.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-03
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_TYPES_H_40F151B6E3A311E2B0650021CCBF5EBE
#define _INCLUDE_PCORE_TYPES_H_40F151B6E3A311E2B0650021CCBF5EBE
#pragma once

// ---- Import configuration macros ----
#include <pcore/compiler.h>

// ---- I expect NULL to be void* type, not integral type ----
#ifndef NULL
# define NULL ((void*)0)
#endif

// ---- Standard integral types ----
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

// ---- Basic true-or-false value representation ----
#ifndef TRUE
# define TRUE 1
#endif
#ifndef FALSE
# define FALSE 0
#endif
typedef int bool;

// ---- Define machine-determined types ----
#ifdef PCORE_X64
  // 64-bit machine word type.
  typedef uint64_t word_t;

  // Signed and unsigned pointer types.
  // Better use void* for general pointers.
  typedef int64_t intptr_t;
  typedef uint64_t uintptr_t;
#else
  // 32-bit machine word type.
  typedef uint32_t word_t;

  // Signed and unsigned pointer types.
  // Better use void* for general pointers.
  typedef int32_t intptr_t;
  typedef uint32_t uintptr_t;
#endif  // PCORE_X64

// ---- Size & Offset types ----
typedef uintptr_t size_t;
typedef intptr_t offset_t;

#endif // _INCLUDE_PCORE_TYPES_H_40F151B6E3A311E2B0650021CCBF5EBE
