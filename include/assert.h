// @file: include/assert.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-06
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_ASSERT_H_8D2AB606E61611E29580810A144E894F
#define _INCLUDE_ASSERT_H_8D2AB606E61611E29580810A144E894F
#pragma once

#if defined(__KERNEL__)
# define assert(X)
#else
# define assert(X) 
#endif

#endif // _INCLUDE_ASSERT_H_8D2AB606E61611E29580810A144E894F
