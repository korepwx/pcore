// @file: kern/mm/buddy_pmm.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-16
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <pcore/pmm.h>
#include <pcore/list.h>
#include <pcore/sync.h>
#include <asm/bitops.h>
#include <stdio.h>

// Whether buddy system is in check.
static int buddy_is_check = 0;

// Buddy system manages page order from 1 to 18
// which, 4K ~ 1G in memory size.
#define BUDDY_MAX_ORDER 18
#define BUDDY_MAX_SIZE (1 << BUDDY_MAX_ORDER)

typedef struct {
  KListEntry  free_list;  // Link list for free area.
  size_t      nr_free;    // Available free pages count.
} FreeArea;
static FreeArea buddy_free_area[BUDDY_MAX_ORDER + 1];

// When try to free the memory, relative offset of the page in memory chunk
// is needed to determine whether the page should be combined.
#define BUDDY_MAX_ZONE 10
typedef struct {
  Page *page_base;
} Zone;
static Zone buddy_zones[BUDDY_MAX_ZONE] = { {0} };

// Shortcut to buddy_free_area[x] properties.
#define free_list(x) (buddy_free_area[x].free_list)
#define nr_free(x) (buddy_free_area[x].nr_free)

// Get 2^(n-1) < X <= 2^n
static inline size_t buddy_getorder(size_t n) {
  if (n < BUDDY_MAX_SIZE)
    return (n > 1) ? ufls(n - 1) : 0;
  kpanic("%u is too large a size for page allocation.\n", n);
}

// Helper function to find out the page index in its belonging zone.
static inline size_t buddy_page2index(Page *page) {
  return page - (buddy_zones[page->zone].page_base);
}

// Helper function to get the page at index.
static inline Page* buddy_index2page(int zone, size_t idx) {
  return buddy_zones[zone].page_base + idx;
}

// Check whether the page is in particular zone and has the desired size.
static inline bool buddy_check_pair(Page *page, int zone, size_t order) {
  if (page2ppn(page) >= pmm_npage) // check whether page is valid.
    return 0;
  return page->zone == zone && !PageHasFlag(page, reserved) 
    && PageHasFlag(page, property) && page->property == order;
}

// ---- Initialize the buddy pmm system ----
static void buddy_init(void) {
  size_t i = 0;
  for (; i <= BUDDY_MAX_ORDER; ++i) {
    nr_free(i) = 0;
    klist_init(&free_list(i));
  }
}

// ---- Insert free memory ----
static void buddy_insert_free_mem(Page* base, size_t n) 
{
  static int zone = 0;
  Page *p = base, *pend = base + n;

  for (; p != pend; ++p) {
    // At first, pmm.c:init_page marks every page to be reserved.
    kassert(PageHasFlag(p, reserved));
    // Zone is used by alloc & free method.
    p->zone = zone;
    // Clear all page flags for initialize.
    p->flags = p->property = 0;
    set_page_ref(p, 0);
  }
  buddy_zones[zone++].page_base = base;

  size_t order = BUDDY_MAX_ORDER, order_size = (1 << order);
  p = base;
  while (n != 0) {
    // Try to fit 2^10, 2^9, ..., 2^0 size of memory into the table.
    while (n >= order_size) {
      p->property = order;
      PageSetFlag(p, property);
      klist_add_before(&free_list(order), &(p->page_link));
      n -= order_size; p += order_size;
      ++nr_free(order);
    }

    // Ready to fit next order_size.
    --order;
    order_size >>= 1;
  }
}

// ---- alloc_pages ----

// Allocate desired order of pages from buddy system.
static Page* buddy_alloc_page_block(size_t order) {
  size_t next_order = order;
  for (; next_order <= BUDDY_MAX_ORDER; ++next_order) {
    if (!klist_empty(&free_list(next_order))) {
      // Found an available page block.
      KListEntry *le = klist_next(&free_list(next_order));
      Page* page = to_struct(le, Page, page_link);
      --nr_free(next_order);
      klist_remove(le);

      // Split the block and give back remaining.
      size_t order_size = 1 << next_order;
      while (next_order > order) {
        --next_order;
        order_size >>= 1;
        Page* pair = page + order_size;
        pair->property = next_order;
        PageSetFlag(pair, property);
        ++nr_free(next_order);
        klist_add_before(&free_list(next_order), &(pair->page_link));
      }

      PageClearFlag(page, property);
      page->property = order;
      return page;
    }
  }

  return NULL;
}

// Allocate desired size of pages from buddy system.
static void buddy_free_pages(Page *base, size_t n);

static Page* buddy_alloc_pages(size_t n) {
  Page *page = NULL;
  {
    if (n == 0)
      goto cleanup;
    
    size_t order = buddy_getorder(n);
    size_t order_size = (1 << order);

    page = buddy_alloc_page_block(order);
    if (page != NULL && n != order_size) {
      buddy_free_pages(page + n, order_size - n);
    }
  }
  
cleanup:
  return page;
}

// ---- Free pages facility ----
// Try to merge one page block into buddy system.
static void buddy_free_page_block(Page *base, size_t order) {
  size_t idx = buddy_page2index(base), pair_idx, size = (1 << order);
  kassert ((idx & (size - 1)) == 0);

  // Check the page status within this block.
  Page *p = base, *pend = base + size;
  for (; p != pend; ++p) {
    kassert (!PageHasFlag(p, reserved) && !PageHasFlag(p, property));
    p->flags = 0;
    set_page_ref(p, 0);
  }

  // Now let's try to merge every possible block.
  int zone = base->zone;
  Page *pair;
  while (order < BUDDY_MAX_ORDER) {
    pair_idx = idx ^ size; // Reverse the index bit.
    pair = buddy_index2page(zone, pair_idx);

    if (!buddy_check_pair(pair, zone, order)) 
      break;

    --nr_free(order);
    klist_remove(&(pair->page_link));
    PageClearFlag(pair, property);
    pair->property = 0;

    idx &= pair_idx; // Clear the last index bit.
    ++order; size <<= 1;
  }

  // Put the merged block into buddy_zones.
  p = buddy_index2page(zone, idx);
  PageSetFlag(p, property);
  p->property = order;
  klist_add_before(&free_list(order), &(p->page_link));
  ++nr_free(order);
}

// Free particular size of pages and put back to buddy sytem.
static void buddy_free_pages(Page *base, size_t n) {
  if (n == 1) {
    buddy_free_page_block(base, 0);
  } else if (n > 0) {
    size_t order = 0, order_size = 1;

    // Free pages from start address to aligned pair.
    while (n >= order_size) {
      kassert (order <= BUDDY_MAX_ORDER);
      if ((buddy_page2index(base) & order_size) != 0) {
        buddy_free_page_block(base, order);
        n -= order_size;
        base += order_size;
      }
      order_size <<= 1;
      ++order;
    }

    // Free pages from aligned pair to end.
    while (n != 0) {
      while (n < order_size) {
        order_size >>= 1;
        --order;
      }
      buddy_free_page_block(base, order);
      base += order_size;
      n -= order_size;
    }
  }
}

// ---- nr_free_pages ----

// Get the maximum free page count in buddy system.
static size_t buddy_nr_free_pages(void) {
  size_t ret = 0, i = 0;
  for (; i <= BUDDY_MAX_ORDER; ++i) {
    ret += nr_free(i) * (1 << i);
  }
  return ret;
}

// ---- Test facility ----
void buddy_print_free(void) {
  size_t order;
  for (order = 0; order <= BUDDY_MAX_ORDER; ++order) {
    KListEntry *head = &(free_list(order)), *le;
    le = klist_next(head);
    printf("Buddy Memory (order=%d, size=0x%x): %d free\n", 
        order, (1 << order), nr_free(order));
    for (; le != head; le = klist_next(le)) {
      Page* page = to_struct(le, Page, page_link);
      printf("  Buddy Chunk (idx=%d, order=%d): 0x%08x\n", 
        buddy_page2index(page), page->property, page2pa(page));
    }
  }
}

// note: buddy_check is derived from ucore.
static void buddy_check(void) 
{
  buddy_is_check = 1;
  
  int i;
  int count = 0, total = 0;
  for (i = 0; i <= BUDDY_MAX_ORDER; i++) {
    KListEntry *list = &free_list(i), *le = list;
    while ((le = klist_next(le)) != list) {
      Page *p = to_struct(le, Page, page_link);
      kassert(PageHasFlag(p, property) && p->property == i);
      count++, total += (1 << i);
    }
  }
  kassert(total == buddy_nr_free_pages());

  Page *p0 = buddy_alloc_pages(8);
  Page *buddy = buddy_alloc_pages(8), *p1;

  kassert(p0 != NULL);
  kassert(!PageHasFlag(p0, property));

  KListEntry free_lists_store[BUDDY_MAX_ORDER + 1];
  unsigned int nr_free_store[BUDDY_MAX_ORDER + 1];

  for (i = 0; i <= BUDDY_MAX_ORDER; i++) {
    free_lists_store[i] = free_list(i);
    klist_init(&free_list(i));
    kassert(klist_empty(&free_list(i)));
    nr_free_store[i] = nr_free(i);
    nr_free(i) = 0;
  }

  kassert(buddy_nr_free_pages() == 0);
  kassert(buddy_alloc_pages(1) == NULL);
  buddy_free_pages(p0, 8);
  kassert(buddy_nr_free_pages() == 8);
  kassert(PageHasFlag(p0, property) && p0->property == 3);
  kassert((p0 = buddy_alloc_pages(6)) != NULL 
          && !PageHasFlag(p0, property) && buddy_nr_free_pages() == 2);

  kassert((p1 = buddy_alloc_pages(2)) != NULL && p1 == p0 + 6);
  kassert(buddy_nr_free_pages() == 0);

  buddy_free_pages(p0, 3);
  kassert(PageHasFlag(p0, property) && p0->property == 1);
  kassert(PageHasFlag(p0 + 2, property) && p0[2].property == 0);

  buddy_free_pages(p0 + 3, 3);
  buddy_free_pages(p1, 2);
  kassert(PageHasFlag(p0, property) && p0->property == 3);

  kassert((p0 = buddy_alloc_pages(6)) != NULL);
  kassert((p1 = buddy_alloc_pages(2)) != NULL);
  buddy_free_pages(p0 + 4, 2);
  buddy_free_pages(p1, 2);

  p1 = p0 + 4;
  kassert(PageHasFlag(p1, property) && p1->property == 2);
  buddy_free_pages(p0, 4);
  kassert(PageHasFlag(p0, property) && p0->property == 3);

  kassert((p0 = buddy_alloc_pages(8)) != NULL);
  kassert(buddy_alloc_pages(1) == NULL && buddy_nr_free_pages() == 0);

  for (i = 0; i <= BUDDY_MAX_ORDER; i++) {
    free_list(i) = free_lists_store[i];
    nr_free(i) = nr_free_store[i];
  }

  buddy_free_pages(p0, 8);
  buddy_free_pages(buddy, 8);

  kassert(total == buddy_nr_free_pages());

  for (i = 0; i <= BUDDY_MAX_ORDER; i++) {
    KListEntry *list = &free_list(i), *le = list;
    while ((le = klist_next(le)) != list) {
      Page *p = to_struct(le, Page, page_link);
      kassert(PageHasFlag(p, property) && p->property == i);
      count--, total -= (1 << i);
    }
  }
  kassert(count == 0);
  kassert(total == 0);
  
  buddy_is_check = 0;
}

// ---- The buddy pmm instance ----
PMManager pmm_buddy_instance = {
  .name             = "Buddy PMManager",
  .init             = buddy_init,
  .insert_free_mem  = buddy_insert_free_mem,
  .alloc_pages      = buddy_alloc_pages,
  .free_pages       = buddy_free_pages,
  .nr_free_pages    = buddy_nr_free_pages,
  .check            = buddy_check,
};

