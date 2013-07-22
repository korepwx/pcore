// @file: include/pcore/pmm.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-12
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_PMM_H_7DA04008EAFC11E280A074E50BEE6214
#define _INCLUDE_PCORE_PMM_H_7DA04008EAFC11E280A074E50BEE6214
#pragma once

#include <stddef.h>
#include <assert.h>
#include <asm/atomic.h>
#include <boot/multiboot.h>

// ---- PMM type declarations ----

/**
 * @brief Forward declaration of Page struct. 
 * If you intend to use Page members, include machine/_pmm.h.
 */
typedef struct _Page Page;

/**
 * @brief Abstract interface for physical memory manager.
 */
struct _PMManager {
  /// @brief Name of the pm manager.
  const char* name;
  
  /// @brief Initialize the pmm instance.
  void (*init)(void);
  
  /// @brief Add newly discovered free memory on initialize.
  void (*insert_free_mem)(Page *base, size_t n);
  
  /**
   * @brief Allocate @n number of pages.
   * @return Address of the first page on success, NULL on failure.
   *    If @n is zero, return NULL as well.
   */
  Page* (*alloc_pages)(size_t n);
  
  /**
   * @brief Free @n number of pages.
   * @note If @n is zero, directly return with no failure.
   */
  void (*free_pages)(Page *base, size_t n);
  
  /// @brief Get the free page count in pm manager.
  size_t (*nr_free_pages)(void);
  
  /// @brief Perform boot-up test on pmm instance.
  void (*check)(void);
};
typedef struct _PMManager PMManager;

// ---- PMM utilities ----

/// @brief Early initialize the physical memory manager via multiboot.
void pmm_multiboot_init(multiboot_info_t* mbi);

/**
 * @brief Initialize the physical memory manager.
 * @return Zero on success, otherwise error.
 */
int pmm_init(void);

// ---- Wrappers for PMManager ----
Page*   kalloc_pages(size_t n);
void    kfree_pages(Page* base, size_t n);
size_t  kfree_page_count(void);

static inline Page* kalloc_page(void) {
  return kalloc_pages(1);
}

static inline void kfree_page(Page* page) {
  kfree_pages(page, 1);
}

// ---- Map memory based IO ports ----
void pmm_iomap(uintptr_t kvaddr, uintptr_t ioaddr, size_t size);

// ---- Include arch depended pmm utilities ----
#include <pcore/memlayout.h>
#include <machine/_pmm.h>

#endif // _INCLUDE_PCORE_PMM_H_7DA04008EAFC11E280A074E50BEE6214
