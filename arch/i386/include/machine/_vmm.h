// @file: arch/i386/include/machine/_vmm.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-18
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _ARCH_I386_INCLUDE_MACHINE_VMM_H_7BCCF350EF7211E2AA2474E50BEE6214
#define _ARCH_I386_INCLUDE_MACHINE_VMM_H_7BCCF350EF7211E2AA2474E50BEE6214
#pragma once

struct _ProcVM
{
  RBTree              mmap_root;        // VMA rbtree root node.
  ProcVMA*            mmap_cache;       // Current accessed vma.
  pde_t*              pgdir;            // PDT of current vm.
  atomic_t            map_count;        // Mapped vma count.
  atomic_t            proc_count;       // Process which share this vm.
  uint32_t            locked_by;        // Owner process id.
};

// ---- Page fault handler ----
int vm_pgfault_handler(ProcVM *mm, uint32_t error_code, uintptr_t addr);

// ---- Fast routines to manipulate ProcVM ----
static inline int vm_get_proc_count(ProcVM *mm) {
  return atomic_read(&(mm->proc_count));
}

static inline void vm_set_proc_count(ProcVM *mm, int val) {
  atomic_set(&(mm->proc_count), val);
}

static inline int vm_inc_proc_count(ProcVM *mm) {
  return atomic_add_return(&(mm->proc_count), 1);
}

static inline int vm_dec_proc_count(ProcVM *mm) {
  return atomic_add_return(&(mm->proc_count), -1);
}

static inline void vm_lock_proc(ProcVM *mm) {
}

static inline void vm_unlock_proc(ProcVM *mm) {
}

#endif // _ARCH_I386_INCLUDE_MACHINE_VMM_H_7BCCF350EF7211E2AA2474E50BEE6214
