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

#if defined(__KERNEL__)

// ---- Compiler inline & noinline optimize ----
#define noinline          __attribute__((noinline))
#define inline            inline __attribute__((always_inline))
  
#endif  // __KERNEL__

#endif // _INCLUDE_PCORE_COMPILER_H_8ED75A6AE3A311E2B48D0021CCBF5EBE
