// @file: include/pcore/picirq.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-17
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_PICIRQ_H_91D41644EEC111E2A322350A8F692308
#define _INCLUDE_PCORE_PICIRQ_H_91D41644EEC111E2A322350A8F692308
#pragma once

#include <stddef.h>

void pic_init(void);
void pic_enable(uint32_t irq);

#endif // _INCLUDE_PCORE_PICIRQ_H_91D41644EEC111E2A322350A8F692308
