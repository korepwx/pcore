// @file: kern/init/init.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-03
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stddef.h>
#include <dev/fb/fb.h>

extern int test_dev_fb();

/**
 * @brief The entry point for pCore.
 * kern_init is called by kern_entry. @sa arch/<platform-depended>/init/entry.S
 */
int kern_init()
{
  va_init();
  test_dev_fb();
  while (1) ;
}

