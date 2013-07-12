// @file: include/stdarg.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-05
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_STDARG_H_9AF5C87CE55B11E2AC8274E50BEE6214
#define _INCLUDE_STDARG_H_9AF5C87CE55B11E2AC8274E50BEE6214
#pragma once

#include <sys/cdefs.h>
#include <sys/_types.h>

// ---- Compiler va-arg support ----
typedef __builtin_va_list va_list;

#define va_start(ap, last) \
  __builtin_va_start((ap), (last))

#define va_arg(ap, type) \
  __builtin_va_arg((ap), type)

#define va_copy(dest, src) \
  __builtin_va_copy((dest), (src))

#define va_end(ap) \
  __builtin_va_end(ap)

#endif // _INCLUDE_STDARG_H_9AF5C87CE55B11E2AC8274E50BEE6214
