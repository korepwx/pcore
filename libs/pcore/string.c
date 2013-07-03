// @file: libs/pcore/string.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-03
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <pcore/string.h>
#include <asm/string.h>

// ---- The following codes are reused from uCore/libs/string.c ----
size_t strlen(const char *s)
{
  size_t cnt = 0;
  while (*s++ != '\0') {
    cnt++;
  }
  return cnt;
}

size_t strnlen(const char *s, size_t len)
{
  size_t cnt = 0;
  while (cnt < len && *s++ != '\0') {
    cnt++;
  }
  return cnt;
}


char *strcpy(char *dst, const char *src)
{
#ifdef __HAVE_ARCH_STRCPY
  return __strcpy(dst, src);
#else
  char *p = dst;
  while ((*p++ = *src++) != '\0')
    /* nothing */ ;
  return dst;
#endif /* __HAVE_ARCH_STRCPY */
}

char *strncpy(char *dst, const char *src, size_t len)
{
  char *p = dst;
  while (len > 0) {
    if ((*p = *src) != '\0') {
      src++;
    }
    p++, len--;
  }
  return dst;
}

char *strcat(char *dst, const char *src)
{
  return strcpy(dst + strlen(dst), src);
}

int strcmp(const char *s1, const char *s2)
{
#ifdef __HAVE_ARCH_STRCMP
  return __strcmp(s1, s2);
#else
  while (*s1 != '\0' && *s1 == *s2) {
    s1++, s2++;
  }
  return (int)((unsigned char)*s1 - (unsigned char)*s2);
#endif /* __HAVE_ARCH_STRCMP */
}

int stricmp(const char *s1, const char *s2)
{
  //while (*s1 != '\0')&& *s1 == *s2) {
  //    s1 ++, s2 ++;
  //}
  char t1, t2;
  while (*s1 != '\0') {
    t1 = ((*s1 > 'Z') ? (*s1 - 32) : *s1);
    t2 = ((*s2 > 'Z') ? (*s2 - 32) : *s2);

    if (t1 == t2) {
      s1++, s2++;
    } else {
      break;
    }
  }
  t1 = ((*s1 > 'Z') ? (*s1 - 32) : *s1);
  t2 = ((*s2 > 'Z') ? (*s2 - 32) : *s2);
  return (int)((unsigned char)t1 - (unsigned char)t2);
}

int strncmp(const char *s1, const char *s2, size_t n)
{
  while (n > 0 && *s1 != '\0' && *s1 == *s2) {
    n--, s1++, s2++;
  }
  return (n == 0) ? 0 : (int)((unsigned char)*s1 - (unsigned char)*s2);
}

int strnicmp(const char* s1, const char* s2, size_t n);

char *strchr(const char *s, char c)
{
  while (*s != '\0') {
    if (*s == c) {
      return (char *)s;
    }
    s++;
  }
  return NULL;
}

char *strfind(const char *s, char c)
{
  while (*s != '\0') {
    if (*s == c) {
      break;
    }
    s++;
  }
  return (char *)s;
}

void *memset(void *s, char c, size_t n)
{
#ifdef __HAVE_ARCH_MEMSET
  return __memset(s, c, n);
#else
  char *p = s;
  while (n-- > 0) {
    *p++ = c;
  }
  return s;
#endif /* __HAVE_ARCH_MEMSET */
}

void *memmove(void *dst, const void *src, size_t n)
{
#ifdef __HAVE_ARCH_MEMMOVE
  return __memmove(dst, src, n);
#else
  const char *s = src;
  char *d = dst;
  if (s < d && s + n > d) {
    s += n, d += n;
    while (n-- > 0) {
      *--d = *--s;
    }
  } else {
    while (n-- > 0) {
      *d++ = *s++;
    }
  }
  return dst;
#endif /* __HAVE_ARCH_MEMMOVE */
}

void *memcpy(void *dst, const void *src, size_t n)
{
#ifdef __HAVE_ARCH_MEMCPY
  return __memcpy(dst, src, n);
#else
  const char *s = src;
  char *d = dst;
  while (n-- > 0) {
    *d++ = *s++;
  }
  return dst;
#endif /* __HAVE_ARCH_MEMCPY */
}

int memcmp(const void *v1, const void *v2, size_t n)
{
  const char *s1 = (const char *)v1;
  const char *s2 = (const char *)v2;
  while (n-- > 0) {
    if (*s1 != *s2) {
      return (int)((unsigned char)*s1 - (unsigned char)*s2);
    }
    s1++, s2++;
  }
  return 0;
}

