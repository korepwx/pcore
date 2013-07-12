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
#include <asm/reg.h>
#include <dev/fb/fb.h>
#include <pcore/bootcons.h>
#include <pcore/kdebug.h>

/// @brief Boot-up message for pCore.
static inline void kern_greeting(void) 
{
  KImageDebugInfo imginfo;
  kdebug_imageinfo(&imginfo);
  
  // Print greetings.
  puts("Bad Apple OS kernel starts loading ...\n\n");
  
  // Print sysinfo.
  printf("Kernel entry at 0x%08zX, memory footprint %zu KB, %s.\n", 
         imginfo.entry_addr, (imginfo.mem_footprint + 1023) / 1024,
         imginfo.optimized ? "optimized" : "non-optimized");
  kdebug_printstackframe();
}

/**
 * @brief The entry point for pCore.
 * kern_init is called by kern_entry. @sa arch/<platform-depended>/init/entry.S
 */
int kern_init()
{
  // Clear the memory space in ELF image.
  extern char edata[], end[];
  memset(edata, 0, end - edata);
  
  // Early initialize the VGA output
  va_init();        // initialize video adapters
  bootcons_init();  // initialize boot console
  
  // Saying that we're loading pCore.
  kern_greeting();
  
  // Loop and prevent the kernel from exit.
  while (1) ;
}

