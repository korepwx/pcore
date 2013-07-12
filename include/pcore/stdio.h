// @file: include/pcore/stdio.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-11
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_STDIO_H_67527C36EA0111E2ADB374E50BEE6214
#define _INCLUDE_PCORE_STDIO_H_67527C36EA0111E2ADB374E50BEE6214
#pragma once

#include <stdio.h>

void kset_putchar(int (*fp)(int));
void kset_getchar(int (*fp)(void));

#if defined(__KERNEL__)
# define kprintf printf
#endif  // __KERNEL__

#endif // _INCLUDE_PCORE_STDIO_H_67527C36EA0111E2ADB374E50BEE6214
