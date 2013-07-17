// @file: kern/dev/fb/fb.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-06
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stddef.h>
#include <assert.h>
#include <errno.h>
#include <dev/fb/fb.h>
#include <dev/fb/vga.h>

// ---- Register all video adapters here ----
static KListEntry va_list;

static inline VideoAdapter *va_get_next_entry(KListEntry *itm) {
  KListEntry *le = klist_next(itm);
  if (le == &va_list) 
    return NULL;
  return ktostruct(le, VideoAdapter, va_list);
}

int va_init() {
  klist_init(&va_list);
  
  // TODO: call probe for all type of devices.
  int ret;
  ret = vga_probe_all();
  
  return ret;
}

VideoAdapter *va_first() {
  return va_get_next_entry(&va_list);
}

VideoAdapter *va_list_open() {
  return va_get_next_entry(&va_list);
}

VideoAdapter *va_list_next(VideoAdapter *va) {
  return va_get_next_entry(&(va->va_list));
}

void va_list_close() {
  // TODO: implement RW lock or RCU lock for va_list.
}

int va_reg_adapter(VideoAdapter* va) {
  if (kissetmask(va->va_flags, V_ADP_REGISTERED))
    return EINVAL;
  ksetmask(va->va_flags, V_ADP_REGISTERED);
  klist_add_before(&va_list, &(va->va_list));
  return 0;
}
