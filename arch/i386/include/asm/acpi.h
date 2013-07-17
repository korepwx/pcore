// @file: arch/i386/include/asm/acpi.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-12
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _ARCH_I386_INCLUDE_ASM_ACPI_H_1B2A2AA0EB0211E2B8CB74E50BEE6214
#define _ARCH_I386_INCLUDE_ASM_ACPI_H_1B2A2AA0EB0211E2B8CB74E50BEE6214
#pragma once

#include <sys/cdefs.h>

// Indicate that current ARCH support "stay idle" halt.
#define PCORE_CPU_SUPPORT_STAY_IDLE 1

/// @brief Turn CPU into "stay idle" state. 
static inline __noreturn void kstay_idle() {
  asm volatile ("hlt;" : : );
  while (1);
}

#endif // _ARCH_I386_INCLUDE_ASM_ACPI_H_1B2A2AA0EB0211E2B8CB74E50BEE6214
