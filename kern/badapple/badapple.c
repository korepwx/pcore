// @file: kern/badapple/badapple.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-18
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <lz4.h>
#include <dev/fb/fb.h>
#include <asm/atomic.h>
#include <asm/io.h>
#include <asm/barrier.h>
#include <pcore/memlayout.h>
#include <pcore/kmalloc.h>

#define DST_WIDTH  640
#define DST_HEIGHT 480
#define FRAME_RATE 30
#define PIC_FRAME_SIZE (DST_WIDTH * DST_HEIGHT / 2)
#define PIC_GROUP_SIZE (FRAME_RATE * PIC_FRAME_SIZE)

extern uint8_t badapple_compressed_video[];

// Playback status.
static uint8_t *p;
static VideoAdapter *va;
static volatile int sched_next_buffer;  // next buffer register used by sched.
static volatile int sched_next_frame;   // next frame used by sched.
static volatile int sched_inited = 0;

// Decode thread switches between second buffers to do the decode.
typedef struct {
  uint8_t *data;
  atomic_t ready;
} SecondBuffer;

static SecondBuffer buffers[2];
static volatile int inuse;

void badapple_main(void)
{
  int i, group = 0;
  
  // Initialize the playback status.
  p = badapple_compressed_video;
  
  // Open VGA display.
  va = va_first();
  va_switch_mode(va, VM_VGA_CG640);
  va_set_palette(va, &kDefault16GrayPalette);
  va_clear_output(va);
  
  // Test image.
  extern uint8_t badapple_compressed_image[];
  //va_video_write(va, badapple_compressed_image, 0, 0, DST_WIDTH, DST_HEIGHT);
  //return;
  
  // Initialize the buffers.
  sched_next_buffer = 0;
  inuse = 0;
  for (i=0; i<2; ++i) {
    atomic_set(&(buffers[i].ready), 0);
    buffers[i].data = kmalloc(PIC_GROUP_SIZE);
    printf("[badapple] buffer[%d] ranges [0x%08x, 0x%08x]\n", i,
           (size_t)buffers[i].data, (size_t)(buffers[i].data + PIC_GROUP_SIZE)
          );
  }
  
  // Test to write in 0xA0000
  char *buff = (char*)(0xA0000 + KERNBASE);
  buff[0] = 0;
  
  // Loop and decode data.
  i = 0;
  sched_inited = 1;
  for (;;) {
    // wait for buffer to be used up.
    while (atomic_read(&(buffers[i].ready)) != 0) 
      io_delay();
    // get next group compressed size.
    int compress_size = *(int*)p;
    p += sizeof(int);
    if (compress_size == 0) {
      va_switch_mode(va, VM_VGA_C80x25);
      va_clear_output(va);
      break;
    }
    // decompress this group.
    int size = LZ4_decompress_safe((const char*)p, (char*)buffers[i].data, 
                                   compress_size, PIC_GROUP_SIZE);
    if (size != PIC_GROUP_SIZE) {
      panic("[badapple] Error decoding %d sec: got %d bytes instead of %d.\n", 
            group, size, PIC_GROUP_SIZE);
    }
    p += compress_size; ++group;
    // Set the ready flag.
    atomic_set(&(buffers[i].ready), 1); 
    printf("[badapple] Decoded %d sec on buffer[%d].\n", group, i);
    i = 1 - i;
  }
}

void badapple_sched(void)
{
  // Check whether next second has been ready.
  if (!sched_inited)
    return;
  if (ktest_and_set_bit(1, &(inuse)) != 0)
    return;
  if (atomic_read(&(buffers[sched_next_buffer].ready)) == 0) {
    kclear_bit(1, &(inuse));
    return;
  }
  
  // Copy current frame out to VGA.
  uint8_t *frame = 
    buffers[sched_next_buffer].data  + PIC_FRAME_SIZE * sched_next_frame;
  va_video_write(va, frame, 0, 0, DST_WIDTH, DST_HEIGHT);
  
  if (++sched_next_frame >= 30) {
    int last_buffer = sched_next_buffer;
    sched_next_buffer = 1 - sched_next_buffer;
    sched_next_frame = 0;
    barrier();
    atomic_set(&(buffers[last_buffer].ready), 0);
  }
  
  kclear_bit(1, &(inuse));
}
