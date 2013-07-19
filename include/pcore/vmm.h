// @file: include/pcore/vmm.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-18
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_VMM_H_7BF2AF6AEF5811E2AF8974E50BEE6214
#define _INCLUDE_PCORE_VMM_H_7BF2AF6AEF5811E2AF8974E50BEE6214
#pragma once

#include <stddef.h>
#include <tree.h>
#include <pcore/memlayout.h>
#include <asm/atomic.h>

/// @brief Forward declartion of virtual memory structure.
typedef struct _ProcVM ProcVM;

/// @brief The virtual memory area.
typedef struct _ProcVMA {
  ProcVM*   vm_proc;
  uintptr_t vm_start;
  uintptr_t vm_end;
  uint32_t  vm_flags;
#define VM_READ                 0x00000001
#define VM_WRITE                0x00000002
#define VM_EXEC                 0x00000004
#define VM_STACK                0x00000008
  RBNode    vm_link;
} ProcVMA;

// ---- Initialize the VMM system ----
void vmm_init(void);

// ---- VM manipulate api ----
ProcVMA* vm_find_vma  (ProcVM *proc, uintptr_t addr);
ProcVMA* vm_create_vma(uintptr_t vm_start, uintptr_t vm_end, uint32_t vm_flags);
void     vm_insert_vma(ProcVM *proc, ProcVMA *vma);

ProcVM*  vm_create_proc (void);
void     vm_destroy_proc(ProcVM *proc);

// ---- Include arch depeneded declarations ----
#include <machine/_vmm.h>


#endif // _INCLUDE_PCORE_VMM_H_7BF2AF6AEF5811E2AF8974E50BEE6214
