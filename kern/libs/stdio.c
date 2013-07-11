// @file: kern/libs/stdio.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-11
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stdio.h>

// ---- 
// putchar & getchar implementation should be replaced by stdin & stdout stream 
// after tty is setup
// ----

static int kputchar_null(int ch) {
  return EOF;
}
static int kgetchar_null(void) {
  return EOF;
}
static int (*kputchar)(int ch)  = kputchar_null;
static int (*kgetchar)(void)    = kgetchar_null;

void kset_putchar(int (*fp)(int)) {
  kputchar = fp;
}

void kget_putchar(int (*fp)(void)) {
  kgetchar = fp;
}

// Implement basic getchar & putchar
int putchar(int ch) {
  return kputchar(ch);
}
