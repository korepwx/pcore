// @file: arch/i386/include/machine/_sync.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-17
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _ARCH_I386_INCLUDE_MACHINE_SYNC_H_C46A1BF0EE9211E28D8D1954E142C93B
#define _ARCH_I386_INCLUDE_MACHINE_SYNC_H_C46A1BF0EE9211E28D8D1954E142C93B
#pragma once

#include <pcore/memlayout.h>
#include <asm/reg.h>
#include <asm/intr.h>

static inline bool __intr_save(void) {
  if (read_eflags() & FL_IF) {
    kintr_disable();
    return 1;
  }
  return 0;
}

static inline void __intr_restore(bool flag) {
  if (flag) {
    kintr_enable();
  }
}

#define local_intr_save(x)      do { x = __intr_save(); } while (0)
#define local_intr_restore(x)   __intr_restore(x);

#endif // _ARCH_I386_INCLUDE_MACHINE_SYNC_H_C46A1BF0EE9211E28D8D1954E142C93B
