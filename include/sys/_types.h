// @file: include/sys/_types.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-05
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_SYS_TYPES_H_E0D9186EE54F11E2A73F74E50BEE6214
#define _INCLUDE_SYS_TYPES_H_E0D9186EE54F11E2A73F74E50BEE6214
#pragma once

#include <bits/wordsize.h>

// ---- Standard integral types ----
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

// ---- Define machine-determined types ----
#if __WORDSIZE == 64

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

#endif  // __WORDSIZE == 64
  
// ---- Size & Offset types ----

typedef intptr_t  ptrdiff_t;
#if !defined(_SIZE_T_DECLARED)
typedef uintptr_t size_t;
typedef intptr_t  ssize_t;
#endif  // _SIZE_T_DECLARED

// ---- Kernel specified types ----
#if defined(__KERNEL__)


#endif // __KERNEL__

#endif // _INCLUDE_SYS_TYPES_H_E0D9186EE54F11E2A73F74E50BEE6214
