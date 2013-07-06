// @file: include/string.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-05
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_STRING_H_DC70A0E6E55211E2BCFE74E50BEE6214
#define _INCLUDE_STRING_H_DC70A0E6E55211E2BCFE74E50BEE6214
#pragma once

#include <sys/cdefs.h>
#include <sys/_null.h>
#include <sys/_types.h>

__BEGIN_DECLS

extern size_t strlen(const char* s);
extern size_t strnlen(const char* s, size_t len);

extern char* strcpy(char *dst, const char *src);
extern char* strncpy(char *dst, const char *src, size_t n);
extern char* strcat(char* dst, const char* src);
//extern char* strncat(char* dst, const char* src, size_t n);

extern int strcmp(const char* s1, const char* s2);
extern int stricmp(const char* s1, const char* s2);
extern int strncmp(const char* s1, const char* s2, size_t n);
extern int strnicmp(const char* s1, const char* s2, size_t n);

extern char *strchr(const char *s, char c);
extern char *strfind(const char *s, char c);

extern void *memset(void *s, char c, size_t n);
extern void *memmove(void *dst, const void *src, size_t n);
extern void *memcpy(void *dst, const void *src, size_t n);
extern int memcmp(const void *v1, const void *v2, size_t n);

__END_DECLS

#endif // _INCLUDE_STRING_H_DC70A0E6E55211E2BCFE74E50BEE6214
