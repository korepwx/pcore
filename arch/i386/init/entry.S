# ---- The main skeleton is derived from ucore ----
#include <pcore/memlayout.h>
#include "multiboot.inc.h"

#define REALLOC(x) (x - KERNBASE)

.text
.globl kern_entry
kern_entry:
  

  # reload temperate gdt (second time) to remap all physical memory
  # virtual_addr 0~4G=linear_addr&physical_addr -KERNBASE~4G-KERNBASE 
  lgdt REALLOC(__gdtdesc)
  movl $KERNEL_DS, %edx
  movw %dx, %ds
  movw %dx, %es
  movw %dx, %ss

  ljmp $KERNEL_CS, $relocated

relocated:

  # set ebp, esp
  movl $0x0, %ebp
  # the kernel stack region is from bootstack -- bootstacktop,
  # the kernel stack size is KSTACKSIZE (8KB)defined in memlayout.h
  movl $bootstacktop, %esp
  # Push multiboot arguments into stack.
  addl  $KERNBASE, %ebx     # Map physical addr to kva.
  pushl %eax
  pushl %ebx
  # now kernel stack is ready , call the first C function
  call kern_init

# should never get here
spin:
  jmp spin

.data
.align PGSIZE
  .globl bootstack
bootstack:
  .space KSTACKSIZE
  .globl bootstacktop
bootstacktop:

.align 4
__gdt:
  SEG_NULL
  SEG_ASM(STA_X | STA_R, - KERNBASE, 0xFFFFFFFF)      # code segment
  SEG_ASM(STA_W, - KERNBASE, 0xFFFFFFFF)              # data segment
__gdtdesc:
  .word 0x17                                          # sizeof(__gdt) - 1
  .long REALLOC(__gdt)

