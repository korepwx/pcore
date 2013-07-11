This is only a student project of OS kernel in C/C++.

I composed this tiny kernel in three major reasons:

  1.  Review the OS lessons in this semester.
  2.  Try to use C++ instead of pure C in OS kernel.
  3.  Implement a kernel video player for Bad Apple.

Some of the codes are derived from https://github.com/chyyuu/ucore_plus
Original LICENSE is unknown. If this matters, please contact me.

Some of the codes are derived from FreeBSD9 under BSD license; 
see LICENSE.freebsd for more details. Other codes are derived from 
GNU C Library; see LICENSE.glibc.

Directory structure:

    arch              Architecture depended code root.
    ..<arch name>     Specified architecture.
    ....include       Arch-headers for user/kernel.
    ......pcore       Arch-headers only for kernel.
    ....libs          Arch-impl for user/kernel.
    boot              The bootloader
    include           Headers for user/kernel.
    ..pcore           Headers only for kernel.
    kern              Impl for kernel.
    ..init            Impl for kernel main skeleton.
    ..libs            Impl for kernel libs.
    libs              Impl for user/kernel.
    tools             Tools for pcore project.
    usr               Impl for user.
    ..libs            Impl for user libs.
    
Use cmake 2.8 to generate Makefile.


