// @file: include/assert.h
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-12
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef _INCLUDE_ASSERT_H_6DDD5AA2EB0111E28CAF74E50BEE6214
#define _INCLUDE_ASSERT_H_6DDD5AA2EB0111E28CAF74E50BEE6214
#pragma once

#include <stddef.h>

void __kpanic(const char* file, int line, const char* fmt, ...) 
  __printf(3, 4) __noreturn;
#define kpanic(...) __kpanic(__FILE__, __LINE__, __VA_ARGS__)

/// @brief Runtime assertion.
#define kassert(X) \
  do { \
    if (!(X)) { \
      kpanic("Assertion failed: %s", #X); \
    } \
  } while(0);

/// @brief The following assertion only works in function scope.
#define kstatic_assert(X) \
  ({extern int __attribute__((error("assertion failure: `" #X "` not true."))) \
      compile_time_check(); ((X)? 0: compile_time_check()), 0; })
      
/// @brief Same as above but support customized message.
#define kstatic_assert_msg(X, MSG) \
  ({extern int __attribute__((error(#MSG))) \
      compile_time_check(); ((X)? 0: compile_time_check()), 0; })
      
/// @brief The following assertion works everywhere but lacks any comments.
#define kstatic_assert_weak(X) \
  { enum { assert_line_ ## __LINE__ = 1/(!!(X)) }; }

#endif // _INCLUDE_ASSERT_H_6DDD5AA2EB0111E28CAF74E50BEE6214
