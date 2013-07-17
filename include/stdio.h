// @file: include/stdio.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-11
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_STDIO_H_E859E408E9F311E2A6A274E50BEE6214
#define _INCLUDE_STDIO_H_E859E408E9F311E2A6A274E50BEE6214
#pragma once

#include <stddef.h>
#include <stdarg.h>

// Basic definitions.
#define EOF (-1)

// ---- Put & Get family methods ----
int putchar(int ch);
int puts(const char* str);

// ---- PrintF family methods ----
int printf  (const char* format, ...) __printf(1, 2);
int vprintf (const char* format, va_list ap);

int sprintf   (char *str, const char* format, ...) __printf(2, 3);
int snprintf  (char *str, size_t size, const char* format, ...) __printf(3, 4);
int vsnprintf (char *str, size_t size, const char* format, va_list ap);

// Prototype for all printf methods.
int printfmt
  (char const *fmt, int (*func)(int, void*), void *arg, int radix, ...)
  __printf(1, 5);
int vprintfmt
  (char const *fmt, int (*func)(int, void*), void *arg, int radix, va_list ap);
  
// Kernel specified utilities 
#if defined(__KERNEL__)
void kset_putchar(int (*fp)(int));
void kset_getchar(int (*fp)(void));
#endif  // __KERNEL__

#endif // _INCLUDE_STDIO_H_E859E408E9F311E2A6A274E50BEE6214
