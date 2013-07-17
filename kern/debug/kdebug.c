// @file: kern/debug/kdebug.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-12
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <pcore/kdebug.h>
#include <pcore/config.h>
#include <string.h>

const char* kdebug_shortpath(const char* path)
{
  int pathlen = strlen(path);
  if (pathlen >= PCORE_BUILD_ROOT_SIZE
      && memcmp(path, PCORE_BUILD_ROOT, PCORE_BUILD_ROOT_SIZE) == 0)
    return path + PCORE_BUILD_ROOT_SIZE;
  return path;
}


