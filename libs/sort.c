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
