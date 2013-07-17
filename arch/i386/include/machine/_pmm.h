// @file: arch/i386/include/machine/_pmm.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-13
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

// This file is derived from ucore/mm/pmm.h

#ifndef _ARCH_I386_INCLUDE_MACHINE_PMM_H_312D35E0EB9311E2927C74E50BEE6214
#define _ARCH_I386_INCLUDE_MACHINE_PMM_H_312D35E0EB9311E2927C74E50BEE6214
#pragma once

// ---- Basic physical memory variables ----
extern Page* pmm_pages;
extern size_t pmm_npage;
extern pde_t *pmm_boot_pgdir;
extern uintptr_t pmm_boot_cr3;
extern pte_t* const pmm_vpt;
extern pde_t* const pmm_vpd;

/* *
 * PADDR - takes a kernel virtual address (an address that points above KERNBASE),
 * where the machine's maximum 256MB of physical memory is mapped and returns the
 * corresponding physical address.  It panics if you pass it a non-kernel virtual address.
 * */
#define PADDR(kva) ({                                                   \
            uintptr_t __m_kva = (uintptr_t)(kva);                       \
            if (__m_kva < KERNBASE) {                                   \
                kpanic("PADDR called with invalid kva %08x", __m_kva);  \
            }                                                           \
            __m_kva - KERNBASE;                                         \
        })

/* *
 * KADDR - takes a physical address and returns the corresponding kernel virtual
 * address. It panics if you pass an invalid physical address.
 * */
#define KADDR(pa) ({                                                    \
            uintptr_t __m_pa = (pa);                                    \
            size_t __m_ppn = PPN(__m_pa);                               \
            if (__m_ppn >= pmm_npage) {                                 \
                kpanic("KADDR called with invalid pa %08x", __m_pa);    \
            }                                                           \
            (void *) (__m_pa + KERNBASE);                               \
        })
        
static inline ppn_t page2ppn(Page *page)
{
  return page - pmm_pages;
}

static inline uintptr_t page2pa(Page *page)
{
  return page2ppn(page) << PGSHIFT;
}

static inline Page *pa2page(uintptr_t pa)
{
  if (PPN(pa) >= pmm_npage) {
    kpanic("pa2page called with invalid pa");
  }
  return &pmm_pages[PPN(pa)];
}

static inline void *page2kva(Page *page)
{
  return KADDR(page2pa(page));
}

static inline Page *kva2page(void *kva)
{
  return pa2page(PADDR(kva));
}

static inline Page *pte2page(pte_t pte) {
  if (!(pte & PTE_P)) {
    kpanic("pte2page called with invalid pte");
  }
  return pa2page(PTE_ADDR(pte));
}

static inline Page *pde2page(pde_t pde) {
  return pa2page(PDE_ADDR(pde));
}

static inline int page_ref(Page *page) {
    return atomic_read(&(page->ref));
}

static inline void set_page_ref(Page *page, int val) {
  atomic_set(&(page->ref), val);
}

static inline int page_ref_inc(Page *page) {
  return atomic_add_return(&(page->ref), 1);
}

static inline int page_ref_dec(Page *page) {
  return atomic_add_return(&(page->ref), -1);
}

/* *
 * load_esp0 - change the ESP0 in default task state segment,
 * so that we can use different kernel stack when we trap frame
 * user to kernel.
 * */
extern void load_esp0(uintptr_t esp0);

// Kernel boot stack & bootstacktop.
extern char bootstack[], bootstacktop[];

// ---- Page table api ----
pte_t *get_pte(pde_t *pgdir, uintptr_t la, bool create);
Page  *get_page(pde_t *pgdir, uintptr_t la, pte_t **ptep_store);

void tlb_invalidate(pde_t *pgdir, uintptr_t la);
void page_remove(pde_t *pgdir, uintptr_t la);
int page_insert(pde_t *pgdir, Page *page, uintptr_t la, uint32_t perm);

void print_pgdir(void);


#endif // _ARCH_I386_INCLUDE_MACHINE_PMM_H_312D35E0EB9311E2927C74E50BEE6214
