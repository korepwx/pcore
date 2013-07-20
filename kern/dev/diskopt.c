// @file: kern/dev/diskopt.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-19
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <string.h>
#include <asm/io.h>
#include <pcore/sync.h>
#include <pcore/diskopt.h>
#include <dev/ide.h>

#define MAX_NSECS 128

// Single sector buffer.
static char secbuf[SECTSIZE] = {0};
  
int kdisk_read(uint16_t ideno, void* buffer, size_t size, size_t offset)
{
  bool intr_flag;
  
  // calculate start & end sector.
  size_t secbeg = offset / SECTSIZE;
  
  // Read the first incomplete sector.
  if (secbeg * SECTSIZE != offset) {
    size_t off = offset - secbeg * SECTSIZE;
    size_t count = (secbeg + 1) * SECTSIZE - offset;
    
    local_intr_save(intr_flag);
    {
      ide_read_secs(ideno, secbeg, secbuf, 1);
      memcpy(buffer, secbuf + off, count);
    }
    local_intr_restore(intr_flag);
    buffer += count; size -= count; ++secbeg;
  }
  
  size_t nsecs = size / SECTSIZE;
  do {
    size_t secnum = (nsecs > MAX_NSECS) ? (MAX_NSECS) : (nsecs);
    
    local_intr_save(intr_flag);
    {
      ide_read_secs(ideno, secbeg, buffer, secnum);
    }
    local_intr_restore(intr_flag);
    
    secbeg += secnum; 
    buffer += SECTSIZE * secnum; 
    size -= SECTSIZE * secnum;
    nsecs -= secnum;
  } while (nsecs > 0);
  
  // Read incomplete last sector.
  if (size > 0) {
    local_intr_save(intr_flag);
    {
      ide_read_secs(ideno, secbeg, secbuf, 1);
      memcpy(buffer, secbuf, size);
    }
    local_intr_restore(intr_flag);
  }
  
  // Return success.
  return 0;
}

int kdisk_read_secs(uint16_t ideno, void* buffer, size_t n, size_t off)
{
  bool intr_flag;
  
  do {
    size_t secnum = (n > MAX_NSECS) ? (MAX_NSECS) : (n);
    
    local_intr_save(intr_flag);
    {
      ide_read_secs(ideno, off, buffer, secnum);
    }
    local_intr_restore(intr_flag);
    
    off += secnum;
    n -= secnum;
    buffer += SECTSIZE * secnum; 
  } while (n > 0);
  
  return 0;
}
