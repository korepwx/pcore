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

/* *
 * STABS debugging info
 *
 * The kernel debugger can understand some debugging information in
 * the STABS format.  For more information on this format, see
 * http://sources.redhat.com/gdb/onlinedocs/stabs_toc.html
 *
 * The constants below define some symbol types used by various debuggers
 * and compilers.  Kernel uses the N_SO, N_SOL, N_FUN, and N_SLINE types.
 * */

#define N_GSYM      0x20  // global symbol
#define N_FNAME     0x22  // F77 function name
#define N_FUN       0x24  // procedure name
#define N_STSYM     0x26  // data segment variable
#define N_LCSYM     0x28  // bss segment variable
#define N_MAIN      0x2a  // main function name
#define N_PC        0x30  // global Pascal symbol
#define N_RSYM      0x40  // register variable
#define N_SLINE     0x44  // text segment line number
#define N_DSLINE    0x46  // data segment line number
#define N_BSLINE    0x48  // bss segment line number
#define N_SSYM      0x60  // structure/union element
#define N_SO        0x64  // main source file name
#define N_LSYM      0x80  // stack variable
#define N_BINCL     0x82  // include file beginning
#define N_SOL       0x84  // included source file name
#define N_PSYM      0xa0  // parameter variable
#define N_EINCL     0xa2  // include file end
#define N_ENTRY     0xa4  // alternate entry point
#define N_LBRAC     0xc0  // left bracket
#define N_EXCL      0xc2  // deleted include file
#define N_RBRAC     0xe0  // right bracket
#define N_BCOMM     0xe2  // begin common
#define N_ECOMM     0xe4  // end common
#define N_ECOML     0xe8  // end common (local name)
#define N_LENG      0xfe  // length of preceding entry

/* Entries in the STABS table are formatted as follows. */
struct stab {
  uint32_t n_strx;  // index into string table of name
  uint8_t n_type;   // type of symbol
  uint8_t n_other;  // misc info (usually empty)
  uint16_t n_desc;  // description field
  uintptr_t n_value;  // value of symbol
};

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
 * 
 * Optimized kernel may not provide accurate stackframes; inline functions 
 * may not report correct line numbers.  So use of this method should take 
 * care.
 */
int noinline kdebug_stackframe(int (*putchar)(int, void*), void* arg);

/// @brief Output stackframe into stdout.
int noinline kdebug_printstackframe(void);

/// @brief Simplify source file path.
const char* kdebug_shortpath(const char* path);

#endif // _INCLUDE_PCORE_KDEBUG_H_CCB123F0EA3011E28BE574E50BEE6214
