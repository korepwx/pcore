// @file: arch/i386/debug/kdebug.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-11
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <string.h>
#include <stdio.h>
#include <pcore/config.h>
#include <pcore/kdebug.h>
#include <pcore/memlayout.h>
#include <pcore/sync.h>
#include <asm/reg.h>
#include <asm/acpi.h>
#include <asm/io.h>
#include <asm/atomic.h>

#define MAX_STACKFRAME_DEPTH 10

// ---- The following codes are derived from ucore/debug/kdebug.c ----
extern const struct stab __STAB_BEGIN__[];  // beginning of stabs table
extern const struct stab __STAB_END__[];  // end of stabs table
extern const char __STABSTR_BEGIN__[];  // beginning of string table
extern const char __STABSTR_END__[];  // end of string table

/* *
 * stab_binsearch - according to the input, the initial value of
 * range [*@region_left, *@region_right], find a single stab entry
 * that includes the address @addr and matches the type @type,
 * and then save its boundary to the locations that pointed
 * by @region_left and @region_right.
 *
 * Some stab types are arranged in increasing order by instruction address.
 * For example, N_FUN stabs (stab entries with n_type == N_FUN), which
 * mark functions, and N_SO stabs, which mark source files.
 *
 * Given an instruction address, this function finds the single stab entry
 * of type @type that contains that address.
 *
 * The search takes place within the range [*@region_left, *@region_right].
 * Thus, to search an entire set of N stabs, you might do:
 *
 *      left = 0;
 *      right = N - 1;    (rightmost stab)
 *      stab_binsearch(stabs, &left, &right, type, addr);
 *
 * The search modifies *region_left and *region_right to bracket the @addr.
 * *@region_left points to the matching stab that contains @addr,
 * and *@region_right points just before the next stab.
 * If *@region_left > *region_right, then @addr is not contained in any
 * matching stab.
 *
 * For example, given these N_SO stabs:
 *      Index  Type   Address
 *      0      SO     f0100000
 *      13     SO     f0100040
 *      117    SO     f0100176
 *      118    SO     f0100178
 *      555    SO     f0100652
 *      556    SO     f0100654
 *      657    SO     f0100849
 * this code:
 *      left = 0, right = 657;
 *      stab_binsearch(stabs, &left, &right, N_SO, 0xf0100184);
 * will exit setting left = 118, right = 554.
 * */
static void
stab_binsearch(const struct stab *stabs, int *region_left, int *region_right,
               int type, uintptr_t addr)
{
  int l = *region_left, r = *region_right, any_matches = 0;

  while (l <= r) {
    int true_m = (l + r) / 2, m = true_m;

    // search for earliest stab with right type
    while (m >= l && stabs[m].n_type != type) {
      m--;
    }
    if (m < l) {  // no match in [l, m]
      l = true_m + 1;
      continue;
    }
    // actual binary search
    any_matches = 1;
    if (stabs[m].n_value < addr) {
      *region_left = m;
      l = true_m + 1;
    } else if (stabs[m].n_value > addr) {
      *region_right = m - 1;
      r = m - 1;
    } else {
      // exact match for 'addr', but continue loop to find
      // *region_right
      *region_left = m;
      l = m;
      addr++;
    }
  }

  if (!any_matches) {
    *region_right = *region_left - 1;
  } else {
    // find rightmost region containing 'addr'
    l = *region_right;
    for (; l > *region_left && stabs[l].n_type != type; l--)
      /* do nothing */ ;
    *region_left = l;
  }
}

/* *
 * kdebug_findmethod - Fill in the @info structure with information about
 * the specified instruction address, @addr.  Returns 0 if information
 * was found, and negative if not.  But even if it returns negative it
 * has stored some information into '*info'.
 * */
int kdebug_findmethod(uintptr_t addr, KMethodDebugInfo *info)
{
  const struct stab *stabs, *stab_end;
  const char *stabstr, *stabstr_end;

  info->filename = "<unknown>";
  info->lineno = 0;
  info->fn_name = "<unknown>";
  info->fn_namelen = 9;
  info->fn_addr = addr;
  info->fn_narg = 0;

  // find the relevant set of stabs
  if (addr >= KERNBASE) {
    stabs = __STAB_BEGIN__;
    stab_end = __STAB_END__;
    stabstr = __STABSTR_BEGIN__;
    stabstr_end = __STABSTR_END__;
  } else {
    return -1;
  }

  // String table validity checks
  if (stabstr_end <= stabstr || stabstr_end[-1] != 0) {
    return -1;
  }
  // Now we find the right stabs that define the function containing
  // 'eip'.  First, we find the basic source file containing 'eip'.
  // Then, we look in that source file for the function.  Then we look
  // for the line number.

  // Search the entire set of stabs for the source file (type N_SO).
  int lfile = 0, rfile = (stab_end - stabs) - 1;
  stab_binsearch(stabs, &lfile, &rfile, N_SO, addr);
  if (lfile == 0)
    return -1;

  // Search within that file's stabs for the function definition
  // (N_FUN).
  int lfun = lfile, rfun = rfile;
  int lline, rline;
  stab_binsearch(stabs, &lfun, &rfun, N_FUN, addr);

  if (lfun <= rfun) {
    // stabs[lfun] points to the function name
    // in the string table, but check bounds just in case.
    if (stabs[lfun].n_strx < stabstr_end - stabstr) {
      info->fn_name = stabstr + stabs[lfun].n_strx;
    }
    info->fn_addr = stabs[lfun].n_value;
    addr -= info->fn_addr;
    // Search within the function definition for the line number.
    lline = lfun;
    rline = rfun;
  } else {
    // Couldn't find function stab!  Maybe we're in an assembly
    // file.  Search the whole file for the line number.
    info->fn_addr = addr;
    lline = lfile;
    rline = rfile;
  }
  info->fn_namelen =
      strfind(info->fn_name, ':') - info->fn_name;

  // Search within [lline, rline] for the line number stab.
  // If found, set info->eip_line to the right line number.
  // If not found, return -1.
  stab_binsearch(stabs, &lline, &rline, N_SLINE, addr);
  if (lline <= rline) {
    info->lineno = stabs[rline].n_desc;
  } else {
    return -1;
  }

  // Search backwards from the line number for the relevant filename stab.
  // We can't just use the "lfile" stab because inlined functions
  // can interpolate code from a different file!
  // Such included source files use the N_SOL stab type.
  while (lline >= lfile
         && stabs[lline].n_type != N_SOL
         && (stabs[lline].n_type != N_SO || !stabs[lline].n_value)) {
    lline--;
  }
  if (lline >= lfile && stabs[lline].n_strx < stabstr_end - stabstr) {
    info->filename = stabstr + stabs[lline].n_strx;
  }
  // Set eip_fn_narg to the number of arguments taken by the function,
  // or 0 if there was no containing function.
  if (lfun < rfun) {
    for (lline = lfun + 1;
         lline < rfun && stabs[lline].n_type == N_PSYM; lline++) {
      info->fn_narg++;
    }
  }
  return 0;
}

// Query about the kernel image info.
int kdebug_imageinfo(KImageDebugInfo *imginfo) 
{
  extern char kern_init[], kern_entry[], end[];
  imginfo->entry_addr = (uintptr_t)kern_init;
  imginfo->mem_footprint = (size_t)(end - kern_entry);
#if defined(__PCORE_NO_OPTIMIZE__)
  imginfo->optimized = 0;
#else
  imginfo->optimized = 1;
#endif  // __PCORE_NO_OPTIMIZE__
  return 0;
}

// Query about the code address after this call.
// note: Because kdebug_readeip doesn't have any parameters, GCC will not 
//       generate "push ebp, mov esp, ebp" here.  So *esp is what we want.
static inline uint32_t read_eip(void)
{
  uint32_t eip;
  asm volatile ("movl 4(%%ebp), %0":"=r" (eip));
  return eip;
}

uintptr_t noinline kdebug_where() 
{
#if defined(__PCORE_NO_OPTIMIZE__)
  return read_eip();
#else
  return *(uintptr_t*)read_esp();
#endif  // __PCORE_NO_OPTIMIZE__
}

// Output stackframe message.
#define PR(ARGS) \
  if ((ret = printfmt ARGS) == EOF) { \
    return EOF; \
  } \
  count += ret;

typedef struct _KStackFrame KStackFrame;
struct _KStackFrame
{
  KStackFrame *next;
  void *retaddr;
};

static int inline kdebug_stackframe_sub
  (int (*putchar)(int, void*), void* arg, const char* prefix)
{
  int i, j;
  int ret, count = 0;
  KMethodDebugInfo info;
  uint32_t ebp = *(uint32_t*)read_ebp(), eip;
  
#if defined(__PCORE_NO_OPTIMIZE__)
  eip = read_eip();
  PR(("%sStackframe traced in non-optimized kernel (eip: 0x%08X):\n", 
      putchar, arg, 10, prefix, eip));
#else
  // find "sub $N, %esp" in kdebug_stackframe in order to get correct 
  // "eip" from caller.  (I at least ensure the eip of on level).
  uint8_t *data = (uint8_t*)kdebug_stackframe;
  j = 0;
  for (i = 0; i < 128; ++i) {
    if (data[i] == 0x81 && data[i+1] == 0xec) {
      j = (int)data[i+2] | ((int)data[i+3] << 8);
      break;
    }
  }
  j += 16;  // Get rid of push eax, push ebx, push ecx & push edx.
  eip = *(uintptr_t*)(read_esp() + j);
#if 1
  eip -= 5; // the instruction to call kdebug_stackframe may be the last 
            // one in a method, so just take the call instruction as eip.
#endif
  // Show message about this address
  PR(("%sStackframe traced in optimized kernel (eip: 0x%08X):\n", 
      putchar, arg, 10, prefix, eip));
#endif  // __PCORE_NO_OPTIMIZE__
  
  i = 0;
  do {
    if (kdebug_findmethod(eip, &info) != 0) {
      PR(("%s  <unknow>: -- 0x%08x --\n", putchar, arg, 10, prefix, eip));
    } else {
      // Obtain correct function name.
      char fnname[256];
      int fnname_len = info.fn_namelen < sizeof(fnname) ? 
                        info.fn_namelen : (sizeof(fnname) - 1);
      memcpy(fnname, info.fn_name, fnname_len);
      fnname[fnname_len] = '\0';
      
      // Dump function info.
      PR(("%s  %s:%d: %s+0x%x\n", putchar, arg, 10, 
          prefix, kdebug_shortpath(info.filename), info.lineno, 
          fnname, (int)(eip - info.fn_addr)));
      
#if defined(__PCORE_NO_OPTIMIZE__)
      // Dump ebp, eip and arguments
      // (On gcc -O2, under most circumstances, these are wrong).
      uint32_t *args = (uint32_t *) ebp + 2;
      PR(("%s    ebp: 0x%08x  eip: 0x%08x", putchar, arg, 10, 
          prefix, ebp, eip));
      if (info.fn_narg > 0) {
        PR(("  args:", putchar, arg, 10));
        for (j=0; j<info.fn_narg; ++j) {
          PR((" 0x%08x", putchar, arg, 10, args[j]));
        }
      }
      PR(("\n", putchar, arg, 10));
#endif  // __PCORE_NO_OPTIMIZE__
    }
    
    count += ret;
    
    // On gcc -O2, ebp may be still 0 (kern_init) after a number of calls.
    if (ebp != 0) {
      eip = ((uint32_t *) ebp)[1];
      ebp = ((uint32_t *) ebp)[0];
    }
  } while (ebp != 0 && ++i <= MAX_STACKFRAME_DEPTH);
  
  // Print "..." if not complete
  if (ebp != 0) {
    PR(("%s  ...\n", putchar, arg, 10, prefix));
  }

  return count;
}

static int kdebug_putchar(int ch, void* arg)
{
  return putchar(ch);
}

int noinline kdebug_stackframe(int (*putchar)(int, void*), void* arg) {
  return kdebug_stackframe_sub(putchar, arg, "");
}

int noinline kdebug_printstackframe(void) {
  return kdebug_stackframe_sub(kdebug_putchar, NULL, "");
}

// ---- kernel panic utility ----
static volatile uint32_t has_dead = 0;

void __kpanic(const char* file, int line, const char* fmt, ...)
{
  kintr_disable();
  if (ktest_and_set_bit(1, &has_dead)) {
    goto die;
  }
  
  va_list ap;
  va_start(ap, fmt);
  printf("Kernel panic at %s:%d\n  ", kdebug_shortpath(file), line);
  vprintf(fmt, ap);
  printf("\n");
  va_end(ap);
  
  // print stackframe.
  kdebug_stackframe_sub(kdebug_putchar, NULL, "  ");
  
die:
#if defined(PCORE_CPU_SUPPORT_STAY_IDLE)
  kstay_idle();
#else
  while (1);
#endif  // PCORE_CPU_SUPPORT_STAY_IDLE
}
