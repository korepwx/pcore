// @file: libs/rand.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-18
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <asm/div64.h>
#include <stdlib.h>

static uint64_t next = 1;

/* *
 * rand - returns a pseudo-random integer
 *
 * The rand() function return a value in the range [0, RAND_MAX].
 * */
int rand(void)
{
  next = (next * 0x5DEECE66DLL + 0xBLL) & ((1LL << 48) - 1);
  uint64_t result = (next >> 12);
  return (int)do_div(result, RAND_MAX + 1);
}

/* *
 * srand - seed the random number generator with the given number
 * @seed:   the required seed number
 * */
void srand(uint32_t seed)
{
  next = seed;
}
