// @file: include/pcore/kmalloc.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-17
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_KMALLOC_H_3F6F42F4EE8D11E28EE0834683352597
#define _INCLUDE_PCORE_KMALLOC_H_3F6F42F4EE8D11E28EE0834683352597
#pragma once

#include <stddef.h>

void kmalloc_init(void);

void *kmalloc(size_t n);
void kfree(void *objp);

#endif // _INCLUDE_PCORE_KMALLOC_H_3F6F42F4EE8D11E28EE0834683352597
