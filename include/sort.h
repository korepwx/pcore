// @file: include/sort.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-16
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_SORT_H_A1E8584AEDBF11E2956874E50BEE6214
#define _INCLUDE_SORT_H_A1E8584AEDBF11E2956874E50BEE6214
#pragma once

#include <stddef.h>

/**
 * @brief Sort the input sequence with insertion algorithm in desc order.
 * @param base: The begin address of sequence.
 * @param nmemb: The element count in sequence.
 * @param size: Per-element size.  No larger than 128.
 * @param compar: Compare two elements. Return 1 when previous is larger, 
 *    0 for equal, and -1 for smaller.
 * @return Zero if success. EINVAL if size > 128.
 */
int insertion_sort(void* base, size_t nmemb, size_t size,
                   int (*compar)(const void*, const void*));

/**
 * @brief Find the first element which is not less than the expected value.
 */
void* lower_bound(void* first, size_t nmemb, size_t size, const void* key, 
                  int (*compar)(const void* v, const void* key));

/**
 * @brief Get the position of expected value in the range.
 */
static inline void* binary_find(void* first, size_t nmemb, size_t size, 
                                const void* key, 
                                int (*compar)(const void* v, const void* key))
{
  first = lower_bound(first, nmemb, size, key, compar);
  return (first != NULL && compar(first, key) == 0) ? first : NULL;
}

#endif // _INCLUDE_SORT_H_A1E8584AEDBF11E2956874E50BEE6214
