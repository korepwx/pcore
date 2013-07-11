// @file: libs/stdio.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-11
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stdio.h>
#include <limits.h>

int puts(const char* str) {
  int ret = 0;
  while (*str) {
    int cret = putchar(*str);
    if (cret == EOF) 
      return EOF;
    ++ret; ++str;
  }
  return ret;
}

#define CALL_PRINTFMT_AP(FMT, PUTCHAR, CTX) \
  int ret; \
  va_list ap; \
  va_start(ap, FMT); \
  ret = printfmt(FMT, (PUTCHAR), (CTX), 10, ap); \
  va_end(ap);

// ---- Console print methods ----
static int vprintf_putchar(int ch, void* arg) 
{
  return putchar(ch);
}

int printf(const char* format, ...)
{
  CALL_PRINTFMT_AP(format, vprintf_putchar, NULL);
  return ret;
}
int vprintf (const char* format, va_list ap)
{
  return printfmt(format, vprintf_putchar, NULL, 10, ap);
}

// ---- String print methods ----
struct vsnprintf_context {
  char *buf;
  size_t written;
  size_t capacity;
};

static int vsnprintf_putchar(int ch, void* arg) 
{
  struct vsnprintf_context *ctx = (struct vsnprintf_context*)(arg);
  if (++ctx->written >= ctx->capacity)
    return EOF;
  ctx->buf[ctx->written] = (char)(ch);
  return 0;
}

#define SPRINTF_INIT_CONTEXT(SIZE) \
  struct vsnprintf_context ctx; \
  ctx.buf = str; \
  ctx.written = 0; \
  ctx.capacity = (SIZE);
  
#define SPRINTF_CALL_PRINTFMT_AP() \
  CALL_PRINTFMT_AP(format, vsnprintf_putchar, &ctx); 
  
#define SPRINTF_CLEANUP() \
  if (ret >= 0) str[ret] = 0; \
  return ret;

int sprintf(char *str, const char* format, ...)
{
  SPRINTF_INIT_CONTEXT(SIZE_MAX);
  SPRINTF_CALL_PRINTFMT_AP();
  SPRINTF_CLEANUP();
}

int snprintf(char *str, size_t size, const char* format, ...)
{
  SPRINTF_INIT_CONTEXT(size);
  SPRINTF_CALL_PRINTFMT_AP();
  SPRINTF_CLEANUP();
}

int vsnprintf(char *str, size_t size, const char* format, va_list ap) 
{
  SPRINTF_INIT_CONTEXT(size);
  int ret = printfmt(format, vsnprintf_putchar, &ctx, 10, ap);
  SPRINTF_CLEANUP();
}

