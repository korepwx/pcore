// @file: include/pcore/compiler.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-03
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_COMPILER_H_8ED75A6AE3A311E2B48D0021CCBF5EBE
#define _INCLUDE_PCORE_COMPILER_H_8ED75A6AE3A311E2B48D0021CCBF5EBE
#pragma once

// ---- Detect compiler & arch settings ----
#if defined(__LP64__) || defined(__64BIT__) || defined(_LP64)
# ifndef PCORE_X64
#  define PCORE_X64
# endif
#endif  // __x64__

#if defined(__PCORE_ARCH_i386__) 
# define PCORE_ARCH_I386
#elif defined(__PCORE_ARCH_x86_64__) || defined(__PCORE_ARCH_ARM__)
# warning Specified architecture is not supported currently.
#else
# error Please specify the build architecture.
#endif  // __PCORE_ARCH_X__

// ---- Compiler detection macros ----
#define GCC_VERSION (__GNUC__ * 10000 \
          + __GNUC_MINOR__ * 100 \
          + __GNUC_PATCHLEVEL__)

// ---- Compiler inline & noinline optimize ----
#define noinline          __attribute__((noinline))
#define inline            inline __attribute__((always_inline))
#define __always_inline   inline __attribute__((always_inline))

// ---- Optimize memory barrier ----
#define barrier()         __asm__ __volatile__("": : :"memory")

// ---- Compiler aid macros ----
#define __pure                __attribute__((pure))
#define __aligned(x)          __attribute__((aligned(x)))
#define __printf(a, b)        __attribute__((format(printf, a, b)))
#define __scanf(a, b)         __attribute__((format(scanf, a, b)))
#define __attribute_const__   __attribute__((__const__))
#define __maybe_unused        __attribute__((unused))
#define __always_unused       __attribute__((unused))

#define __packed              __attribute__((packed))

// ---- Round operation macros ----

// Round down to the nearest multiple of N. 
// Apply this macro on negative values will cause unspecified behaviours.
#define ROUND_DOWN(X, N) ({                             \
  (typeof(X))((X) - (X) % (N));                         \
});

// Round up to the nearest multiple of N.
// Apply this macro on negative values will cause unspecified behaviours.
#define ROUND_UP(X, N) ({                               \
  (typeof(X))(ROUND_DOWN((X) + (N) - 1, (N)))           \
});

// Round up the result of dividing N.
#define ROUND_UP_DIV(X, N) ({                           \
  (typeof(X))(((X) + (N) - 1) / (N))                    \
});

// ---- "Magic" struct manipulate macros ----

/// @brief get the offset of member in a struct
#define OFFSET_OF(TYPE, MEMBER)                         \
  ((size_t)(&((TYPE*)0)->MEMBER))

/**
 * @brief   get struct from a pointer 
 * @ptr:    a pointer to struct member
 * @type:   the struct type which contains the member
 * @member: the name of member in the struct
 */
#define TO_STRUCT(PTR, TYPE, MEMBER)                    \
  ((TYPE*)((char*)(PTR) - OFFSET_OF(TYPE, MEMBER)))

#endif // _INCLUDE_PCORE_COMPILER_H_8ED75A6AE3A311E2B48D0021CCBF5EBE
