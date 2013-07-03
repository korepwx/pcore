# @file: CMakeLists.txt
# Copyright (c) 2013 Korepwx. All rights reserved.
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Contributors:
#   Korepwx  <public@korepwx.com>  2013-07-02
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Use of this source code is governed by a BSD-style license that can be found
# in the LICENSE file

cmake_minimum_required(VERSION 2.8)
project(pcore)

###
# pCore Configuration
#
SET (PCORE_ARCH i386)
ADD_DEFINITIONS (-D__PCORE_ARCH_${PCORE_ARCH}__=1)

###
# Common Cmake switches
#

# Setup pCore include directory.
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_LIST_DIR}/include)
INCLUDE_DIRECTORIES (arch/${PCORE_ARCH}/include)

# Compiler flags.
SET (CMAKE_C_FLAGS "-g -Wall -O2 -ffreestanding -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc -fno-stack-protector")
SET (CMAKE_CXX_FLAGS "-g -Wall -O2 -ffreestanding -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc -fno-stack-protector -fno-exceptions -fno-rtti")
SET (CMAKE_ASM_FLAGS "-g -Wall -O2 -ffreestanding -fno-builtin -Wall -ggdb -m32 -gstabs -nostdinc -fno-stack-protector")
#SET (CMAKE_EXE_LINKER_FLAGS "-m elf_i386 -nostdlib")

# Enable ASM (AT&T)
ENABLE_LANGUAGE(C ASM)

###
# pCore general libraries
#
FILE (GLOB_RECURSE GENERAL_SRCS libs/*.c libs/*.cc libs/*.S)
FILE (
  GLOB_RECURSE GENERAL_ARCH_SRCS 
  arch/${PCORE_ARCH}/libs/*.c
  arch/${PCORE_ARCH}/libs/*.cc
  arch/${PCORE_ARCH}/libs/*.S
)

ADD_LIBRARY(
  baselib STATIC
  ${GENERAL_SRCS}
  ${GENERAL_ARCH_SRCS}
)

###
# pCore BootLoader
#

FILE (GLOB BOOTLOADER_SRCS boot/*.c boot/*.cc boot/*.S)

ADD_LIBRARY (
  bootloader STATIC 
  ${BOOTLOADER_SRCS}
)

ADD_CUSTOM_TARGET (
  bootblock.img ALL
  DEPENDS bootloader
  COMMAND 
    ld -m elf_i386 -T${CMAKE_CURRENT_LIST_DIR}/tools/boot.ld -o ${CMAKE_BINARY_DIR}/bootloader.img ${CMAKE_BINARY_DIR}/libbootloader.a &&
    objcopy --only-keep-debug bootloader.img bootloader.sym &&
    objcopy -S -O binary ${CMAKE_BINARY_DIR}/bootloader.img ${CMAKE_BINARY_DIR}/bootloader.img &&
    python ${CMAKE_CURRENT_LIST_DIR}/tools/mkboot.py ${CMAKE_BINARY_DIR}/bootloader.img ${CMAKE_BINARY_DIR}/bootblock.img
)

###
# pCore Kernel
#

FILE (
  GLOB_RECURSE KERN_SRCS 
  kern/*.c kern/*.cc kern/*.S
)
FILE (
  GLOB_RECURSE KERN_ARCH_SRCS 
  arch/${PCORE_ARCH}/*.c 
  arch/${PCORE_ARCH}/*.cc
  arch/${PCORE_ARCH}/*.S
)

IF (${GENERAL_SRCS})
  LIST(REMOVE_ITEM KERN_SRCS ${GENERAL_SRCS})
ENDIF()
IF (${GENERAL_ARCH_SRCS})
  LIST(REMOVE_ITEM KERN_ARCH_SRCS ${GENERAL_ARCH_SRCS})
ENDIF()

ADD_LIBRARY (
  kernel STATIC
  ${KERN_SRCS}
  ${KERN_ARCH_SRCS}
)

ADD_CUSTOM_TARGET (
  kernel.img ALL
  DEPENDS kernel baselib
  COMMAND 
    ld -m elf_i386 -T${CMAKE_CURRENT_LIST_DIR}/tools/kernel.ld -o kernel.img ${CMAKE_BINARY_DIR}/libkernel.a ${CMAKE_BINARY_DIR}/libbaselib.a &&
    objcopy --only-keep-debug kernel.img kernel.sym
)

###
# Disk image for qemu emulator
#
ADD_CUSTOM_TARGET (
  pcore.img ALL
  DEPENDS kernel.img bootblock.img
  COMMAND
    dd if=/dev/zero of=${CMAKE_BINARY_DIR}/pcore.img count=10000 &&
    dd if=${CMAKE_BINARY_DIR}/bootblock.img of=${CMAKE_BINARY_DIR}/pcore.img obs=512 conv=notrunc &&
    dd if=${CMAKE_BINARY_DIR}/kernel.img of=${CMAKE_BINARY_DIR}/pcore.img seek=1 obs=512 conv=notrunc
)

###
# Launch qemu emulator commands
#
SET(QEMU_OPTS 
  -hda ${CMAKE_BINARY_DIR}/pcore.img
)

ADD_CUSTOM_TARGET(
  qemu
  DEPENDS pcore.img
  COMMAND
    qemu-system-i386 -parallel stdio ${QEMU_OPTS} -serial null 
)

ADD_CUSTOM_TARGET(
  debug
  DEPENDS pcore.img
  COMMAND
    cp ${CMAKE_CURRENT_LIST_DIR}/tools/gdbinit ${CMAKE_BINARY_DIR}/gdbinit &&
    cp ${CMAKE_CURRENT_LIST_DIR}/tools/run-gdb.sh ${CMAKE_BINARY_DIR}/run-gdb.sh &&
    cd ${CMAKE_BINARY_DIR} && ./run-gdb.sh
)
