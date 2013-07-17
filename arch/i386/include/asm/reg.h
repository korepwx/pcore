// @file: arch/i386/include/asm/reg.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-11
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _ARCH_I386_INCLUDE_ASM_REG_H_07458790EA3111E2998C74E50BEE6214
#define _ARCH_I386_INCLUDE_ASM_REG_H_07458790EA3111E2998C74E50BEE6214
#pragma once

#include <stddef.h>
#include <pcore/memlayout.h>
#include <pcore/compiler.h>

#define PCORE_DEFINE_READ_REG(REG, TYPE, TYPEF, BARRIER) \
  static inline TYPE read_ ## REG (void) { \
    TYPE ret; \
    asm volatile ("mov" #TYPEF " %%" #REG ", %0" : "=r" (ret) : : BARRIER); \
    return ret; \
  }
  
#define PCORE_DEFINE_LOAD_REG(REG, TYPE, TYPEF, BARRIER) \
  static inline void write_ ## REG (TYPE v) { \
    asm volatile ("mov" #TYPEF " %0, %%" #REG :: "r" (v) : BARRIER); \
  }
  
PCORE_DEFINE_READ_REG(ebp, uint32_t,  l, );
PCORE_DEFINE_READ_REG(esp, uint32_t,  l, );
PCORE_DEFINE_READ_REG(cr0, uintptr_t,  , "memory");
PCORE_DEFINE_READ_REG(cr1, uintptr_t,  , "memory");
PCORE_DEFINE_READ_REG(cr2, uintptr_t,  , "memory");
PCORE_DEFINE_READ_REG(cr3, uintptr_t,  , "memory");

PCORE_DEFINE_LOAD_REG(cr0, uintptr_t,  , "memory");
PCORE_DEFINE_LOAD_REG(cr1, uintptr_t,  , "memory");
PCORE_DEFINE_LOAD_REG(cr2, uintptr_t,  , "memory");
PCORE_DEFINE_LOAD_REG(cr3, uintptr_t,  , "memory");

static inline void ltr(uint16_t sel) {
  asm volatile ("ltr %0" :: "r" (sel) : "memory");
}

static inline void lidt(struct lgdtdesc *pd) {
  asm volatile ("lidt (%0)" :: "r" (pd) : "memory");
}

static inline void invlpg(void *addr) {
  asm volatile ("invlpg (%0)" :: "r" (addr) : "memory");
}

static inline uint32_t read_eflags(void) {
  uint32_t eflags;
  asm volatile ("pushfl; popl %0" : "=r" (eflags));
  return eflags;
}

static inline void write_eflags(uint32_t eflags) {
  asm volatile ("pushl %0; popfl" :: "r" (eflags));
}

/* *
 * lgdt - load the global descriptor table register and reset the
 * data/code segement registers for kernel.
 * */
static inline void lgdt(struct lgdtdesc *pd) {
  asm volatile ("lgdt (%0)" :: "r" (pd));
  asm volatile ("movw %%ax, %%gs" :: "a" (USER_DS));
  asm volatile ("movw %%ax, %%fs" :: "a" (USER_DS));
  asm volatile ("movw %%ax, %%es" :: "a" (KERNEL_DS));
  asm volatile ("movw %%ax, %%ds" :: "a" (KERNEL_DS));
  asm volatile ("movw %%ax, %%ss" :: "a" (KERNEL_DS));
  // reload cs
  asm volatile ("ljmp %0, $1f\n 1:\n" :: "i" (KERNEL_CS));
}

#endif // _ARCH_I386_INCLUDE_ASM_REG_H_07458790EA3111E2998C74E50BEE6214
