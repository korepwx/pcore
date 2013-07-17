// @file: include/pcore/serial.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-17
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_PCORE_SERIAL_H_3152CB02EEC711E2BC60CFD46AA2C47D
#define _INCLUDE_PCORE_SERIAL_H_3152CB02EEC711E2BC60CFD46AA2C47D
#pragma once

/// @brief Initialize the serial port.
void serial_init(void);

/// @brief Put character into serial port.
void serial_putchar(int ch);

#endif // _INCLUDE_PCORE_SERIAL_H_3152CB02EEC711E2BC60CFD46AA2C47D
