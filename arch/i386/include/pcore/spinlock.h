// @file: arch/i386/include/pcore/spinlock.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-19
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _ARCH_I386_INCLUDE_PCORE_SPINLOCK_H_1CAD41B4F04D11E28C0574E50BEE6214
#define _ARCH_I386_INCLUDE_PCORE_SPINLOCK_H_1CAD41B4F04D11E28C0574E50BEE6214
#pragma once

#include <asm/atomic.h>

/// @brief General spinlock type.
typedef struct {
  volatile int flag;  // Whether this spinlock has been acquired.
} SpinLock;

/// @brief Initialize a spinlock.
void spinlock_init(SpinLock* lock) {
  lock->flag = 0;
}

/// @brief Try to lock a spinlock.
bool spinlock_trylock(SpinLock* lock) {
  return ktest_and_set_bit(1, &(lock->flag)) == 0;
}

/// @brief Unlock a spinlock.
void spinlock_unlock(SpinLock* lock) {
  kclear_bit(1, &(lock->flag));
}

/// @brief Lock a spinlock, and wait if still acquired by other.
void spinlock_lock(SpinLock* lock) {
  while (ktest_and_set_bit(1, &(lock->flag)) == 1);
}

#endif // _ARCH_I386_INCLUDE_PCORE_SPINLOCK_H_1CAD41B4F04D11E28C0574E50BEE6214
