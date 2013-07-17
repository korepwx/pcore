// @file: arch/i386/include/asm/intr.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-17
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _ARCH_I386_INCLUDE_ASM_INTR_H_3C03FA9CEE9211E2AB996D5891C8A938
#define _ARCH_I386_INCLUDE_ASM_INTR_H_3C03FA9CEE9211E2AB996D5891C8A938
#pragma once

/* intr_enable - enable irq interrupt */
static inline void kintr_enable(void) {
  asm volatile ("sti");
}

/* intr_disable - disable irq interrupt */
static inline void kintr_disable(void) {
  asm volatile ("cli" ::: "memory");
}

#endif // _ARCH_I386_INCLUDE_ASM_INTR_H_3C03FA9CEE9211E2AB996D5891C8A938
