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
#include <string.h>
#include <lz4.h>
#include <dev/fb/fb.h>
#include <asm/atomic.h>
#include <asm/io.h>
#include <asm/barrier.h>
#include <pcore/memlayout.h>
#include <pcore/kmalloc.h>
#include <pcore/badapple_config.h>
#include <pcore/diskopt.h>
#include <pcore/spinlock.h>

#define DST_WIDTH  640
#define DST_HEIGHT 480
#define FRAME_RATE 30
#define PIC_FRAME_SIZE (DST_WIDTH * DST_HEIGHT / 2)
#define PIC_PLANE_SIZE (PIC_FRAME_SIZE / 4)
#define PIC_GROUP_SIZE (FRAME_RATE * PIC_FRAME_SIZE)

// Playback status.
static VideoAdapter *va;
static volatile int sched_skip_frames;  // skipped frames by sched.
static volatile int sched_next_buffer;  // next buffer register used by sched.
static volatile int sched_next_frame;   // next frame used by sched.
static volatile int sched_inited = 0;

// Decode thread switches between second buffers to do the decode.
typedef struct {
  uint8_t *data;
  atomic_t ready;
} SecondBuffer;

static SecondBuffer buffers[2];

SpinLock sched_running;       // Indicate whether any sched is running.
SpinLock sched_skip_counter;  // Lock on the sched_skip_frames.

void badapple_main(void)
{
  int i;
  
  // Open VGA display.
  va = va_first();
  va_switch_mode(va, VM_VGA_CG640);
  va_set_palette(va, &kDefault16GrayPalette);
  va_clear_output(va);
  
  // Initialize the buffers.
  sched_next_buffer = 0;
  sched_skip_frames = 0;
  spinlock_init(&sched_running);
  spinlock_init(&sched_skip_counter);
  
  for (i=0; i<2; ++i) {
    atomic_set(&(buffers[i].ready), 0);
    buffers[i].data = kmalloc(PIC_GROUP_SIZE);
  }
  
  // Display loading image.
  uint8_t *badapple_loading_data = badapple_loading_image;
  do {
    int compress_size = *(int*)badapple_loading_data;
    int size = 
      LZ4_decompress_safe(
        (const char*)badapple_loading_data + sizeof(int),
        (char*)buffers[0].data, compress_size, PIC_FRAME_SIZE
      );
    if (size != PIC_FRAME_SIZE) {
      panic("[badapple] Error decoding image: got %d bytes instead of %d.\n", 
            size, PIC_FRAME_SIZE);
    }
    
    va_video_write(va, buffers[0].data, 0, 0, DST_WIDTH, DST_HEIGHT);
  } while (0);
  
  // Create video data memory jar.
  uint8_t* video_compressed_data = (uint8_t*)kmalloc(BADAPPLE_VIDEO_DATA_SIZE);
  printf("[badapple] Video compressed data in memory [0x%08x, 0x%08x].\n",
         (size_t)video_compressed_data, 
         (size_t)(video_compressed_data + BADAPPLE_VIDEO_DATA_SIZE)
  );

  // Load video data from external disk.
  if (kdisk_read(0, video_compressed_data, BADAPPLE_VIDEO_DATA_SIZE, 
                 BADAPPLE_VIDEO_DATA_OFFSET) != 0) {
    panic("[badapple] Cannot load video data from disk.\n");
  }
  printf("[badapple] Video data already loaded from disk.\n");
  
  // Loop and decode data.
  do {
    int group = 0;
    uint8_t *p = video_compressed_data;
    i = 0; sched_inited = 1;
    for (;;) {
      // wait for buffer to be used up.
      while (atomic_read(&(buffers[i].ready)) != 0) 
        io_delay();
      // get next group compressed size.
      int compress_size = *(int*)p;
      p += sizeof(int); ++group;
      if (compress_size == 0) {
        va_clear_output(va);
        break;
      }
      
      // decompress this group.
      int size = LZ4_decompress_safe((const char*)p, (char*)buffers[i].data, 
                                     compress_size, PIC_GROUP_SIZE);
      if (size != PIC_GROUP_SIZE) {
        printf("[badapple] Error decoding %d sec (offset 0x%08x): "
               "got %d bytes instead of %d; compress size is %d.\n", 
               group, p - video_compressed_data, size, PIC_GROUP_SIZE,
               compress_size
              );
      }
      
#if 0
      else {
        printf("[badapple] Decoded %d sec on buffer[%d].\n", group, i);
      }
#endif

      p += compress_size;
      // Set the ready flag.
      atomic_set(&(buffers[i].ready), 1);
      i = 1 - i;
    }
  } while (0);
  
  // Free all resources.
  kfree(video_compressed_data);
}

void badapple_sched(void)
{
  // Check whether next second has been ready.
  if (!sched_inited)
    return;
  if (!spinlock_trylock(&sched_running)) {
    spinlock_lock(&sched_skip_counter);
    ++sched_skip_frames;
    spinlock_unlock(&sched_skip_counter);
    return;
  }
  if (atomic_read(&(buffers[sched_next_buffer].ready)) == 0) {
    //printf("[badapple] warn: decode not finished.\n");
    goto return_unlock;
  }
  
  // Copy current frame out to VGA.
  uint8_t *frame = 
    buffers[sched_next_buffer].data  + PIC_FRAME_SIZE * sched_next_frame;
    
#if 0
  va_video_write(va, frame, 0, 0, DST_WIDTH, DST_HEIGHT);
#endif

  // Since I have already encoded video in plane mode, so memcpy to output.
  int m;
  for (m=0; m<4; ++m, frame += PIC_PLANE_SIZE) {
    va_set_plane_mask(va, (1 << m));
    memcpy(va->va_buffer, frame, PIC_PLANE_SIZE);
  }
  
  if (++sched_next_frame >= 30) {
    int last_buffer = sched_next_buffer;
    sched_next_buffer = 1 - sched_next_buffer;
    
    // Deal with skipped frames.
    spinlock_lock(&sched_skip_counter);
    if (sched_skip_frames >= 30) {
      sched_next_frame = 29;
      sched_skip_frames -= 29;
    } else {
      sched_next_frame = sched_skip_frames;
      sched_skip_frames = 0;
    }
    spinlock_unlock(&sched_skip_counter);
    if (sched_next_frame > 0) {
      printf("[badapple] warn: Skip %d frames.\n", sched_next_frame);
    }
    
    // Notify the decoder that the buffer has already been used up.
    barrier();
    atomic_set(&(buffers[last_buffer].ready), 0);
  }
  
return_unlock:
  spinlock_unlock(&sched_running);
}
