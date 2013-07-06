// @file: include/ctype.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-05
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_CTYPE_H_AE8EC494E55911E292AD74E50BEE6214
#define _INCLUDE_CTYPE_H_AE8EC494E55911E292AD74E50BEE6214
#pragma once

#include <sys/cdefs.h>
#include <sys/_types.h>

__BEGIN_DECLS

// Methods without a table.
static inline int islower(int c) {
  return (c >= 'a' && c <= 'z');
}

static inline int isupper(int c) {
  return (c >= 'A' && c <= 'Z');
}

static inline int isdigit(int c) {
  return (c >= '0' && c <= '9');
}

static inline int isalpha(int c) {
  return isupper(c) || islower(c);
}

static inline int isalnum(int c) {
  return isalpha(c) || isdigit(c);
}


static inline int isspace(int c) {
  return c == ' ' || c == '\f' || c == '\n'
         || c == '\r' || c == '\t' || c == '\v';
}

static inline int isxdigit(int c) {
  return (c >= '0' && c <= '9')
         || (c >= 'a' && c <= 'f')
         || (c >= 'A' && c <= 'F');
}

static inline int isblank(int c) {
  return c == ' ' || c == '\t';
}

static inline int tolower(int c) {
  return isupper(c) ? (c + 32) : c;
}

static inline int toupper(int c) {
  return islower(c) ? (c - 32) : c;
}

// Methods with a character table.
// Table should be declared in libs/ctype.c
#if 0
static inline int iscntrl(int c);
static inline int isgraph(int c);
static inline int isprint(int c);
static inline int ispunct(int c);
#endif

__END_DECLS

#endif // _INCLUDE_CTYPE_H_AE8EC494E55911E292AD74E50BEE6214
