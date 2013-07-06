// @file: arch/i386/include/pcore/memlayout.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-03
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _ARCH_I386_INCLUDE_PCORE_MEMLAYOUT_H_E3A1C674E3AE11E2B87A0021CCBF5EBE
#define _ARCH_I386_INCLUDE_PCORE_MEMLAYOUT_H_E3A1C674E3AE11E2B87A0021CCBF5EBE
#pragma once

// ---- The following code is derived from uCore ----

#if defined(__KERNEL__)

/* global segment number */
#define SEG_KTEXT   1
#define SEG_KDATA   2
#define SEG_UTEXT   3
#define SEG_UDATA   4
#define SEG_TSS     5

/* global descrptor numbers */
#define GD_KTEXT    ((SEG_KTEXT) << 3)  // kernel text
#define GD_KDATA    ((SEG_KDATA) << 3)  // kernel data
#define GD_UTEXT    ((SEG_UTEXT) << 3)  // user text
#define GD_UDATA    ((SEG_UDATA) << 3)  // user data
#define GD_TSS      ((SEG_TSS) << 3)  // task segment selector

#define DPL_KERNEL  (0)
#define DPL_USER    (3)

#define KERNEL_CS   ((GD_KTEXT) | DPL_KERNEL)
#define KERNEL_DS   ((GD_KDATA) | DPL_KERNEL)
#define USER_CS     ((GD_UTEXT) | DPL_USER)
#define USER_DS     ((GD_UDATA) | DPL_USER)


/* *
 * Virtual memory map:                                          Permissions
 *                                                              kernel/user
 *
 *     4G ------------------> +---------------------------------+
 *                            |                                 |
 *                            |         Empty Memory (*)        |
 *                            |                                 |
 *                            +---------------------------------+ 0xFB000000
 *                            |   Cur. Page Table (Kern, RW)    | RW/-- PTSIZE
 *     VPT -----------------> +---------------------------------+ 0xFAC00000
 *                            |        Invalid Memory (*)       | --/--
 *     KERNTOP -------------> +---------------------------------+ 0xF8000000
 *                            |                                 |
 *                            |    Remapped Physical Memory     | RW/-- KMEMSIZE
 *                            |                                 |
 *     KERNBASE ------------> +---------------------------------+ 0xC0000000
 *                            |        Invalid Memory (*)       | --/--
 *     USERTOP -------------> +---------------------------------+ 0xB0000000
 *                            |           User stack            |
 *                            +---------------------------------+
 *                            |                                 |
 *                            :                                 :
 *                            |         ~~~~~~~~~~~~~~~~        |
 *                            :                                 :
 *                            |                                 |
 *                            ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *                            |       User Program & Heap       |
 *     UTEXT ---------------> +---------------------------------+ 0x00800000
 *                            |        Invalid Memory (*)       | --/--
 *                            |  - - - - - - - - - - - - - - -  |
 *                            |    User STAB Data (optional)    |
 *     USERBASE, USTAB------> +---------------------------------+ 0x00200000
 *                            |        Invalid Memory (*)       | --/--
 *     0 -------------------> +---------------------------------+ 0x00000000
 * (*) Note: The kernel ensures that "Invalid Memory" is *never* mapped.
 *     "Empty Memory" is normally unmapped, but user programs may map pages
 *     there if desired.
 *
 * */

/* All physical memory mapped at this address */
#define KERNBASE            0xC0000000
#define KMEMSIZE            0x38000000  // the maximum amount of physical memory
#define KERNTOP             (KERNBASE + KMEMSIZE)

/* *
 * Virtual page table. Entry PDX[VPT] in the PD (Page Directory) contains
 * a pointer to the page directory itself, thereby turning the PD into a page
 * table, which maps all the PTEs (Page Table Entry) containing the page mappings
 * for the entire virtual address space into that 4 Meg region starting at VPT.
 * */
#define VPT                 0xFAC00000

#define KSTACKPAGE          2 // # of pages in kernel stack
#define KSTACKSIZE          (KSTACKPAGE * PGSIZE) // sizeof kernel stack

#define USERTOP             0xB0000000
#define USTACKTOP           USERTOP
#define USTACKPAGE          256 // # of pages in user stack
#define USTACKSIZE          (USTACKPAGE * PGSIZE) // sizeof user stack

#define USERBASE            0x00200000
#define UTEXT               0x00800000  // where user programs generally begin
#define USTAB               USERBASE  // the location of the user STABS data structure

#define USER_ACCESS(start, end)                     \
    (USERBASE <= (start) && (start) < (end) && (end) <= USERTOP)

#define KERN_ACCESS(start, end)                     \
    (KERNBASE <= (start) && (start) < (end) && (end) <= KERNTOP)
    
// ---- pCore hacked utilities ----
#define KVADDR(pa)        ((pa) + KERNBASE)

#endif  // __KERNEL__
    
#endif // _ARCH_I386_INCLUDE_PCORE_MEMLAYOUT_H_E3A1C674E3AE11E2B87A0021CCBF5EBE
