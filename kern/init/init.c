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
#include <asm/acpi.h>
#include <dev/fb/fb.h>
#include <pcore/bootcons.h>
#include <pcore/kdebug.h>
#include <pcore/config.h>
#include <pcore/pmm.h>
#include <pcore/trap.h>
#include <pcore/picirq.h>
#include <pcore/clock.h>
#include <pcore/sync.h>
#include <pcore/serial.h>
#include <assert.h>

/// @brief Boot-up message for pCore.
static inline void kern_greeting(void) 
{
  KImageDebugInfo imginfo;
  kdebug_imageinfo(&imginfo);
  
  // Print greetings.
  puts("Bad Apple OS v0.1 starts loading ...\n\n");
#if defined(PCORE_WAIT_ON_BOOT_GREETING)
  int i;
  for (i=0; i<50000000; ++i)
    io_delay();
#endif  // PCORE_WAIT_ON_BOOT_GREETING
  
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
  serial_init();    // initialize the serial ports.
  bootcons_init();  // initialize boot console
  
  // Saying that we're loading pCore.
  kern_greeting();
  
  // Initialize physical memory management.
  pmm_init();
  
  // Intialize the trap system.
  trap_init();
  
  // Initialize the hardware interrupts.
  pic_init();
  
  // Initialize the system clock.
  clock_init();
  
  // Enable the irq interrupts.
  kintr_enable();
  
  // Loop and prevent the kernel from exit.
  kstay_idle();
}

