// @file: include/pcore/trap.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-17
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_TRAP_H_B021E43CEE9E11E2A94185B946FA92E2
#define _INCLUDE_PCORE_TRAP_H_B021E43CEE9E11E2A94185B946FA92E2
#pragma once

#include <stddef.h>
#include <pcore/memlayout.h>

/// @brief Abstracted TrapFrame instance.
typedef struct _TrapFrame TrapFrame;

/// @brief Initialize the interrupt system.
void trap_init(void);

/// @brief Print the trap frame into stdout.
void trap_printframe(TrapFrame *tf);

// ---- Include the arch depended declarations ----
#include <machine/_trap.h>

#endif // _INCLUDE_PCORE_TRAP_H_B021E43CEE9E11E2A94185B946FA92E2
