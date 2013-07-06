// @file: include/stddef.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-05
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_STDDEF_H_F9B2E3E8E55311E2836074E50BEE6214
#define _INCLUDE_STDDEF_H_F9B2E3E8E55311E2836074E50BEE6214
#pragma once

#include <sys/cdefs.h>
#include <sys/_null.h>
#include <sys/_types.h>

#ifndef __cplusplus

typedef uint16_t wchar_t;
typedef int bool;
#define true 1
#define false 0

#endif  // !__cplusplus

/// @brief get the offset of member in a struct
#define offsetof(TYPE, MEMBER)                          \
  ((size_t)(&((TYPE*)0)->MEMBER))
  
/**
 * @brief   get struct from a pointer 
 * @ptr:    a pointer to struct member
 * @type:   the struct type which contains the member
 * @member: the name of member in the struct
 */
#define to_struct(PTR, TYPE, MEMBER)                    \
  ((TYPE*)((char*)(PTR) - offsetof(TYPE, MEMBER)))

#endif // _INCLUDE_STDDEF_H_F9B2E3E8E55311E2836074E50BEE6214
