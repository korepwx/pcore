// @file: arch/i386/trap/trap.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-17
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

// This file is derived from ucore/trap/trap.c

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <asm/reg.h>
#include <pcore/trap.h>
#include <pcore/sync.h>
#include <pcore/clock.h>
#include <pcore/sched.h>
#include <pcore/memlayout.h>

/* *
 * Interrupt descriptor table:
 *
 * Must be built at run time because shifted function addresses can't
 * be represented in relocation records.
 * */
static struct gatedesc idt[256] = {{0}};

static struct lgdtdesc idt_pd = {
  sizeof(idt) - 1, (uintptr_t)idt
};

/**
 * @brief initialize IDT to each of the entry points in trap/vectors.S
 * This function turns on CPU trap in x86.
 */
void trap_init(void) {
  extern uintptr_t __trapvectors[];
  int gateid;
  
  // Initialize all gates according to assembly IDT vectors.
  //
  // Each item in IDT vectors is such a piece of instructions: pushes the  
  // gateid into stack and call __alltraps (in trapentry.S).
  // Then __alltraps initialize the structure of trapframe and call trap.
  //
  // And then trap_dispatch is called to dispatch the trap (as a static method, 
  // gcc will properly inline it into trap).
  //
  // All gates are considered as an interrupt, and thus run in kernel mode.
  for (gateid = 0; gateid < karraysize(idt); ++gateid) {
    SETGATE(idt[gateid], 1, GD_KTEXT, __trapvectors[gateid], DPL_KERNEL);
  }
  
  // Specially set the system call interrupt exception gate.
  // NOTE: The assignment requires syscall to use "exception" as type, and 
  //       "trap" as privilege. I cannot understand what it means. 
  SETGATE(idt[T_SYSCALL], 1, GD_KTEXT, __trapvectors[T_SYSCALL], DPL_USER);
  
  // Setup up the IDT table.
  lidt(&idt_pd);
}

// ---- Trap dispatch details ----
static void trap_dispatch(TrapFrame *tf) 
{
  switch (tf->tf_trapno) {
    // Page fault.
    case T_PGFLT:
      trap_printframe(tf);
      kpanic("Unexpected page fault in kernel.");
      break;
    // Timer ticks.
    case IRQ_OFFSET + IRQ_TIMER:
      clock_inc();
      sched_run_timers();
      break;
    // IDE disks.
    case IRQ_OFFSET + IRQ_IDE1:
    case IRQ_OFFSET + IRQ_IDE2:
      /* do nothing */
      break;
    // Unexpected trap.
    default:
      trap_printframe(tf);
      kpanic("Unexpected trap in kernel.");
      break;
  }
}

/**
 * @brief handles or dispatches an exception/interrupt. 
 * if and when trap()  returns, the code in trap/trapentry.S restores the 
 * old CPU state saved in the trapframe and then uses the iret instruction 
 * to return from the exception.
 */
void trap(TrapFrame *tf) 
{
  trap_dispatch(tf);
  
#if 0
  // dispatch based on what type of trap occurred
  // used for previous projects
  if (current == NULL) {
    trap_dispatch(tf);
  }
  else {
    // keep a trapframe chain in stack
    TrapFrame *otf = current->tf;
    current->tf = tf;

    bool in_kernel = trap_in_kernel(tf);

    trap_dispatch(tf);

    current->tf = otf;
    if (!in_kernel) {
        if (current->flags & PF_EXITING) {
            do_exit(-E_KILLED);
        }
        if (current->need_resched) {
            schedule();
        }
    }
  }
#endif 
}

// ---- Trap dump details ----
static const char* trapname(int trapno) {
  static const char * const excnames[] = {
    "Divide error",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "BOUND Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection",
    "Page Fault",
    "(unknown trap)",
    "x87 FPU Floating-Point Error",
    "Alignment Check",
    "Machine-Check",
    "SIMD Floating-Point Exception"
  };

  if (trapno < karraysize(excnames)) {
    return excnames[trapno];
  }
  if (trapno >= IRQ_OFFSET && trapno < IRQ_OFFSET + 16) {
    return "Hardware Interrupt";
  }
  return "(unknown trap)";
}

static const char *IA32flags[] = {
  "CF", NULL, "PF", NULL, "AF", NULL, "ZF", "SF",
  "TF", "IF", "DF", "OF", NULL, NULL, "NT", NULL,
  "RF", "VM", "AC", "VIF", "VIP", "ID", NULL, NULL,
};

static void print_regs(TrapPushRegs *regs) {
  printf("  edi  0x%08x\n", regs->reg_edi);
  printf("  esi  0x%08x\n", regs->reg_esi);
  printf("  ebp  0x%08x\n", regs->reg_ebp);
  printf("  oesp 0x%08x\n", regs->reg_oesp);
  printf("  ebx  0x%08x\n", regs->reg_ebx);
  printf("  edx  0x%08x\n", regs->reg_edx);
  printf("  ecx  0x%08x\n", regs->reg_ecx);
  printf("  eax  0x%08x\n", regs->reg_eax);
}

void trap_printframe(TrapFrame *tf) {
  bool intr_flag;
  local_intr_save(intr_flag);
  {
    printf("TrapFrame at 0x%08x\n", (size_t)tf);
    print_regs(&tf->tf_regs);
    printf("  ds   0x----%04x\n", tf->tf_ds);
    printf("  es   0x----%04x\n", tf->tf_es);
    printf("  fs   0x----%04x\n", tf->tf_fs);
    printf("  gs   0x----%04x\n", tf->tf_gs);
    printf("  trap 0x%08x %s\n", tf->tf_trapno, trapname(tf->tf_trapno));
    printf("  err  0x%08x\n", tf->tf_err);
    printf("  eip  0x%08x\n", tf->tf_eip);
    printf("  cs   0x----%04x\n", tf->tf_cs);
    printf("  flag 0x%08x ", tf->tf_eflags);

    int i, j;
    for (i = 0, j = 1; i < karraysize(IA32flags); i ++, j <<= 1) {
      if ((tf->tf_eflags & j) && IA32flags[i] != NULL) {
        printf("%s,", IA32flags[i]);
      }
    }

    printf("IOPL=%d\n", (tf->tf_eflags & FL_IOPL_MASK) >> 12);

    if (!trap_in_kernel(tf)) {
      printf("  esp  0x%08x\n", tf->tf_esp);
      printf("  ss   0x----%04x\n", tf->tf_ss);
    }
  }
  local_intr_restore(intr_flag);
}
