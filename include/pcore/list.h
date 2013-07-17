// @file: include/pcore/list.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-06
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_LIST_H_10CA3FE0E61211E2A8D5E5C2B71A9077
#define _INCLUDE_PCORE_LIST_H_10CA3FE0E61211E2A8D5E5C2B71A9077
#pragma once

#include <stddef.h>

// ---- This file contains a generic kernel list implementation ----
struct _KListEntry {
  struct _KListEntry *prev, *next;
};
typedef struct _KListEntry KListEntry;

/**
 * @brief Initliaze a kernel list with no items.
 */
static __always_inline void klist_init(KListEntry* list) {
  list->prev = list->next = list;
}

static __always_inline void 
    klist_add_before(KListEntry* item, KListEntry* itemToAdd) {
  itemToAdd->prev = item->prev;
  itemToAdd->next = item;
  item->prev->next = itemToAdd;
  item->prev = itemToAdd;
}

static __always_inline void 
    klist_add_after(KListEntry* item, KListEntry* itemToAdd) {
  klist_add_before(item->next, itemToAdd);
}

static __always_inline void 
    klist_add(KListEntry*item, KListEntry* itemToAdd) {
  klist_add_before(item, itemToAdd);
}

static __always_inline void klist_remove(KListEntry* item) {
  item->prev->next = item->next;
  item->next->prev = item->prev;
}

static __always_inline bool klist_empty(KListEntry* list) {
  return (list == list->next);
}

static __always_inline KListEntry* klist_next(KListEntry* item) {
  return item->next;
}

static __always_inline KListEntry* klist_prev(KListEntry* item) {
  return item->prev;
}

#endif // _INCLUDE_PCORE_LIST_H_10CA3FE0E61211E2A8D5E5C2B71A9077
