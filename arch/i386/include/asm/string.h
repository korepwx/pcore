// @file: arch/i386/include/asm/string.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-03
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file. Some code is reused from uCore, see README.ucore for
// more details.

#ifndef _ARCH_I386_INCLUDE_ASM_STRING_H_741266E6E3AA11E2AEDF0021CCBF5EBE
#define _ARCH_I386_INCLUDE_ASM_STRING_H_741266E6E3AA11E2AEDF0021CCBF5EBE
#pragma once

// ---- The following codes are reused from uCore/arch/i386/libs/arch.h ----
#ifndef __HAVE_ARCH_STRCMP
#define __HAVE_ARCH_STRCMP
static inline int __strcmp(const char *s1, const char *s2)
{
  int d0, d1, ret;
  asm volatile ("1: lodsb;"
          "scasb;"
          "jne 2f;"
          "testb %%al, %%al;"
          "jne 1b;"
          "xorl %%eax, %%eax;"
          "jmp 3f;"
          "2: sbbl %%eax, %%eax;"
          "orb $1, %%al;" "3:":"=a" (ret), "=&S"(d0), "=&D"(d1)
          :"1"(s1), "2"(s2)
          :"memory");
  return ret;
}

#endif /* __HAVE_ARCH_STRCMP */

#ifndef __HAVE_ARCH_STRCPY
#define __HAVE_ARCH_STRCPY
static inline char *__strcpy(char *dst, const char *src)
{
  int d0, d1, d2;
  asm volatile ("1: lodsb;"
          "stosb;"
          "testb %%al, %%al;"
          "jne 1b;":"=&S" (d0), "=&D"(d1), "=&a"(d2)
          :"0"(src), "1"(dst):"memory");
  return dst;
}
#endif /* __HAVE_ARCH_STRCPY */

#ifndef __HAVE_ARCH_MEMSET
#define __HAVE_ARCH_MEMSET
static inline void *__memset(void *s, char c, size_t n)
{
  int d0, d1;
  asm volatile ("rep; stosb;":"=&c" (d0), "=&D"(d1)
          :"0"(n), "a"(c), "1"(s)
          :"memory");
  return s;
}
#endif /* __HAVE_ARCH_MEMSET */

#ifndef __HAVE_ARCH_MEMCPY
#define __HAVE_ARCH_MEMCPY
static inline void *__memcpy(void *dst, const void *src, size_t n)
{
  int d0, d1, d2;
  asm volatile ("rep; movsl;"
          "movl %4, %%ecx;"
          "andl $3, %%ecx;"
          "jz 1f;"
          "rep; movsb;" "1:":"=&c" (d0), "=&D"(d1), "=&S"(d2)
          :"0"(n / 4), "g"(n), "1"(dst), "2"(src)
          :"memory");
  return dst;
}
#endif /* __HAVE_ARCH_MEMCPY */

#ifndef __HAVE_ARCH_MEMMOVE
#define __HAVE_ARCH_MEMMOVE
static inline void *__memmove(void *dst, const void *src, size_t n)
{
  if (dst < src) {
    return __memcpy(dst, src, n);
  }
  int d0, d1, d2;
  asm volatile ("std;"
          "rep; movsb;" "cld;":"=&c" (d0), "=&S"(d1), "=&D"(d2)
          :"0"(n), "1"(n - 1 + src), "2"(n - 1 + dst)
          :"memory");
  return dst;
}
#endif /* __HAVE_ARCH_MEMMOVE */

#endif // _ARCH_I386_INCLUDE_ASM_STRING_H_741266E6E3AA11E2AEDF0021CCBF5EBE
