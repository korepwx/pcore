// @file: kern/init/init.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-03
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <asm/io.h>
#include <dev/fb/fb.h>
#include <pcore/boot_cons.h>

/**
 * @brief The entry point for pCore.
 * kern_init is called by kern_entry. @sa arch/<platform-depended>/init/entry.S
 */
int kern_init()
{
  // Clear the memory space in PE image.
  extern char edata[], end[];
  memset(edata, 0, end - edata);
  
  // Early initialize the VGA output
  va_init();        // initialize video adapters.
  boot_cons_init(); // initialize boot console
  
  // Saying that we're loading.
  puts("pCore loading ...\n");
  
  // Loop and prevent the kernel from exit.
  while (1) ;
}

