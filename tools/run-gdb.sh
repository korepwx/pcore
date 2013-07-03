#!/bin/bash
QEMU_OPTS="-hda pcore.img"
PWD=`pwd`
qemu-system-i386 -S -gdb tcp::1234 -parallel stdio ${QEMU_OPTS} -serial null&
sleep 2&
konsole --workdir="${PWD}" -e gdb -q -x gdbinit
