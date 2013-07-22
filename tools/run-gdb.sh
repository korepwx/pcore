#!/bin/bash
QEMU_OPTS="-m 300M -kernel kernel.img -soundhw ac97,hda,pcspk -hda badapple.img"
PWD=`pwd`
qemu-system-i386 -S -gdb tcp::1234 -parallel stdio ${QEMU_OPTS} -serial null&
sleep 2&
konsole --workdir="${PWD}" -e gdb -q -x gdbinit
