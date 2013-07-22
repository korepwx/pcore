// @file: libs/sort.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-16
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <sort.h>
#include <string.h>
#include <errno.h>

#define SORT_MAX_ELEMENT_SIZE 128

int insertion_sort(void* base, size_t nmemb, size_t size,
                   int (*compar)(const void*, const void*))
{
  char buf[SORT_MAX_ELEMENT_SIZE];
  void *last = base + nmemb * size;
  void *it, *min;
  
  if (size > SORT_MAX_ELEMENT_SIZE) 
    return EINVAL;
  
  while (last != base) {
    min = base; it = base + size;
    while (it != last) {
      if (compar(it, min) > 0) {
        min = it;
      }
      it += size;
    }
    if (base != min) {
      memcpy(buf, base, size);
      memcpy(base, min, size);
      memcpy(min, buf, size);
    }
    base += size;
  }
  
  return 0;
}

void* lower_bound(void* first, size_t nmemb, size_t size, const void* key, 
                  int (*compar)(const void*, const void*))
{
  ptrdiff_t p;
  void *it, *last = first + nmemb * size;
  void *nil = last;
  
  // We want to search for the last *it < v in the range [first, last).
  // + If last == first, we return last (indicating not found).
  // + If last == first + 1, we return last if (*first < v), else first.
  while (last > first) {
    // Otherwise we start to examine [first, last)
    p = ((last - first) / size) / 2;
    it = first + p * size; // it should be at least first + 1 if not the above situations.
    // Here we consider [first, .., it, .., last).
    if (compar(it, key) < 0) {
      // Range is now [it+1, ..., last).
      it += size;
      first = it;
    } else {
      // Range is now [first, .., it), where *it >= v.
      last = it;
    }
  }
  
  return (first != nil) ? (first) : (NULL);
}
