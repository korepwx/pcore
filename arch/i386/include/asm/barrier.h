// @file: arch/i386/include/asm/barrier.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-19
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _ARCH_I386_INCLUDE_ASM_BARRIER_H_ACF88A6CF03411E2AFF074E50BEE6214
#define _ARCH_I386_INCLUDE_ASM_BARRIER_H_ACF88A6CF03411E2AFF074E50BEE6214
#pragma once

static void inline barrier(void) {
  asm volatile ("" : : : "memory"); 
}

#endif // _ARCH_I386_INCLUDE_ASM_BARRIER_H_ACF88A6CF03411E2AFF074E50BEE6214
