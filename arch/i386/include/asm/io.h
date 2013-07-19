// @file: arch/i386/include/asm/io.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-04
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _ARCH_I386_INCLUDE_ASM_IO_H_BBF7EDC8E4BD11E2A86974E50BEE6214
#define _ARCH_I386_INCLUDE_ASM_IO_H_BBF7EDC8E4BD11E2A86974E50BEE6214
#pragma once

#include <stddef.h>
#include <pcore/compiler.h>

// ---- Port IO instructions ----

static inline void io_delay(void) {
  asm volatile ("outb %al, $0xED");
}

#define PCORE_MAKE_PORT_IO(bw, type)                                  \
  static inline unsigned type in##bw(uint16_t port)                   \
  {                                                                   \
    unsigned type value;                                              \
    asm volatile ("in" #bw " %1, %0"                                  \
                  : "=a" (value) : "d" (port));                       \
    return value;                                                     \
  }                                                                   \
  static inline void out##bw(uint16_t port, unsigned type value)      \
  {                                                                   \
    asm volatile ("out" #bw " %0, %1"                                 \
                  : : "a" (value), "d" (port));                       \
  }                                                                   \
  static inline unsigned type in##bw##_p(uint16_t port)               \
  {                                                                   \
    unsigned type value = in##bw(port);                               \
    io_delay();                                                       \
    return value;                                                     \
  }                                                                   \
  static inline void out##bw##_p(uint16_t port, unsigned type value)  \
  {                                                                   \
    out##bw(port, value);                                             \
    io_delay();                                                       \
  }
    
PCORE_MAKE_PORT_IO(b, char)
PCORE_MAKE_PORT_IO(w, short)

// TODO: merge insl into above.
static inline void insl(uint32_t port, void *addr, int cnt)
{
  asm volatile ("cld;" "repne; insl;":"=D" (addr), "=c"(cnt)
          :"d"(port), "0"(addr), "1"(cnt)
          :"memory", "cc");
}

static inline void outsl(uint32_t port, const void *addr, int cnt) {
  asm volatile (
      "cld;"
      "repne; outsl;"
      : "=S" (addr), "=c" (cnt)
      : "d" (port), "0" (addr), "1" (cnt)
      : "memory", "cc");
}

#endif // _ARCH_I386_INCLUDE_ASM_IO_H_BBF7EDC8E4BD11E2A86974E50BEE6214
