// @file: include/dev/ide.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-19
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_DEV_IDE_H_B5E92798F05311E29D8474E50BEE6214
#define _INCLUDE_DEV_IDE_H_B5E92798F05311E29D8474E50BEE6214
#pragma once

#include <stddef.h>

#define SECTSIZE 512

void ide_init(void);
bool ide_device_valid(uint16_t ideno);
size_t ide_device_size(uint16_t ideno);

int ide_read_secs(uint16_t ideno, uint32_t secno, void *dst, size_t nsecs);
int ide_write_secs(uint16_t ideno, uint32_t secno, const void *src, size_t nsecs);

#endif // _INCLUDE_DEV_IDE_H_B5E92798F05311E29D8474E50BEE6214
