// @file: include/pcore/kdebug.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-11
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_KDEBUG_H_CCB123F0EA3011E28BE574E50BEE6214
#define _INCLUDE_PCORE_KDEBUG_H_CCB123F0EA3011E28BE574E50BEE6214
#pragma once

#include <stddef.h>
#include <pcore/compiler.h>

// ---- The following are routines for debug information ----

/**
 * @brief Debug information about the kernel image.
 */
struct _KImageDebugInfo {
  uintptr_t entry_addr;     // Kernel virtual address of kern_entry.
  size_t mem_footprint;     // Kernel memory footprint (in bytes).
  bool optimized;           // Whether this kernel is optimized?
};
typedef struct _KImageDebugInfo KImageDebugInfo;

/**
 * @brief Debug information about methods in an ELF image.
 */
struct _KMethodDebugInfo {
  const char *filename;     // source code filename
  int lineno;               // source code line number
  const char *fn_name;      // name of function containing eip
  int fn_namelen;           // length of function's name
  uintptr_t fn_addr;        // start address of function
  int fn_narg;              // number of function arguments
};
typedef struct _KMethodDebugInfo KMethodDebugInfo;

/**
 * @brief Query about a method of which entry is at specified address.
 * @return 0 if success, -1 if failed.
 */
int kdebug_findmethod(uintptr_t addr, KMethodDebugInfo *info);

/// @brief Query about the kernel image info.
int kdebug_imageinfo(KImageDebugInfo *imginfo);

/// @brief Query about the code address after this call.
uintptr_t noinline kdebug_where();

/**
 * @brief Print current stack frame.
 * @param putchar: Message character output method.
 * @param arg: Argument to be passed to putchar.
 * @return Character output, or -1 if error.
 * 
 * Stack frames may differ greatly from one arch to another.  So instead 
 * of provide a structure for stackframe, I just let this method output 
 * stackframe trace message.
 */
int noinline kdebug_stackframe(int (*putchar)(int, void*), void* arg);

/// @brief Debug stdout character output message.
int kdebug_putchar(int ch, void* arg);

/// @brief Output stackframe into stdout.
static inline int kdebug_printstackframe(void) {
  return kdebug_stackframe(kdebug_putchar, NULL);
}

#endif // _INCLUDE_PCORE_KDEBUG_H_CCB123F0EA3011E28BE574E50BEE6214
