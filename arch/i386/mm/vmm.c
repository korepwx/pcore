// @file: arch/i386/mm/vmm.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-18
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <pcore/pmm.h>
#include <pcore/vmm.h>
#include <pcore/kmalloc.h>
#include <tree.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

// ---- Generate RB tree codes ----
#define rbn2vma(VAR, MEMBER) \
  to_struct((VAR), ProcVMA, MEMBER)

static int vma_compare(RBNode* a, RBNode* b) {
  ptrdiff_t ret = rbn2vma(a, vm_link)->vm_start 
    - rbn2vma(b, vm_link)->vm_start;
  if (ret > 0) 
    return 1;
  else if (ret < 0) 
    return -1;
  return 0;
}

static int vma_key_compare(RBNode* n, void* key) {
  ptrdiff_t ret = rbn2vma(n, vm_link)->vm_start
    - *(uintptr_t*)key;
  if (ret > 0)
    return 1;
  else if (ret < 0)
    return -1;
  return 0;
}

// ---- ProcVM & ProcVMA basic interfaces ----
ProcVM* vm_create_proc(void)
{
  ProcVM *proc = kmalloc(sizeof(ProcVM));
  
  if (proc != NULL) {
    rb_tree_init(&(proc->mmap_root), vma_compare);
    proc->mmap_cache = NULL;
    proc->pgdir = NULL;
    proc->map_count.counter = 0;
    
    vm_set_proc_count(proc, 0);
    proc->locked_by = 0;
  }
  
  return proc;
}

ProcVMA* vm_create_vma(uintptr_t vm_start, uintptr_t vm_end, uint32_t vm_flags)
{
  ProcVMA *vma = kmalloc(sizeof(ProcVMA));
  if (vma != NULL) {
    vma->vm_start = vm_start;
    vma->vm_end = vm_end;
    vma->vm_flags = vm_flags;
  }
  return vma;
}

ProcVMA* vm_find_vma(ProcVM *proc, uintptr_t addr)
{
  ProcVMA *vma = NULL;
  
  if (proc != NULL) {
    vma = proc->mmap_cache;
    if (!(vma != NULL && vma->vm_start <= addr && vma->vm_end > addr)) {
      RBNode *ret =
        rb_upbound(&(proc->mmap_root), vma_key_compare, &addr);
      if (ret == NULL)
        vma = NULL;
      else {
        vma = rbn2vma(ret, vm_link);
        if (vma->vm_end <= addr)
          vma = NULL;
      }
    }
    if (vma != NULL)
      proc->mmap_cache = vma;
  }
  return vma;
}

static inline void vma_check_overlap(ProcVMA *prev, ProcVMA *next) {
  kassert(prev->vm_start < prev->vm_end);
  kassert(prev->vm_end <= next->vm_start);
  kassert(next->vm_start < next->vm_end);
}

void vm_insert_vma(ProcVM *proc, ProcVMA *vma) {
  kassert(vma->vm_start < vma->vm_end);
  rb_insert(&(proc->mmap_root), &(vma->vm_link));
  
#if defined(__PCORE_NO_OPTIMIZE__)
  // Check overlap.
  RBNode *x = rb_search(&(proc->mmap_root), vma_key_compare, &(vma->vm_start));
  kassert(x != NULL);
  
  RBNode *y = rb_node_prev(&(proc->mmap_root), x);
  if (y != NULL) {
    vma_check_overlap(rbn2vma(y, vm_link), vma);
  }
  
  y = rb_node_next(&(proc->mmap_root), x);
  if (y != NULL) {
    vma_check_overlap(vma, rbn2vma(y, vm_link));
  }
#endif  // __PCORE_NO_OPTIMIZE__
  atomic_add(&(proc->map_count), 1);
}

void vm_destroy_proc(ProcVM *proc)
{
  kassert(vm_get_proc_count(proc) == 0);
  
  // Find the left-most node.
  RBNode *start = rb_leftmost(&(proc->mmap_root));
  
  // Destroy every node.
  for (; start != NULL;) {
    RBNode *right = rb_node_next(&(proc->mmap_root), start);
    kfree(rbn2vma(start, vm_link));
    start = right;
  }
  
  kfree(proc);
}

// ---- Initialize the VMM system ----

static void print_vma_struct(ProcVM *proc) {
  // Find the left-most node.
  RBNode *start = rb_leftmost(&(proc->mmap_root));
  
  // Print every node.
  for (; start != NULL;) {
    ProcVMA *vma = rbn2vma(start, vm_link);
    printf("VMA start=%08u, end=%08u\n", vma->vm_start, vma->vm_end);
    start = rb_node_next(&(proc->mmap_root), start);
  }
  
  kfree(proc);
}

static void check_vma_struct(void) {
  ProcVM *mm = vm_create_proc();
  assert(mm != NULL);

  int step1 = 10, step2 = step1 * 10;

  int i;
  for (i = step1; i >= 1; i --) {
    ProcVMA *vma = vm_create_vma(i * 5, i * 5 + 2, 0);
    assert(vma != NULL);
    vm_insert_vma(mm, vma);
  }

  for (i = step1 + 1; i <= step2; i ++) {
    ProcVMA *vma = vm_create_vma(i * 5, i * 5 + 2, 0);
    assert(vma != NULL);
    vm_insert_vma(mm, vma);
  }

  RBNode *rbn = rb_leftmost(&(mm->mmap_root));

  for (i = 1; i <= step2; i ++) {
    assert(rbn != NULL);
    ProcVMA *mmap = rbn2vma(rbn, vm_link);
    assert(mmap->vm_start == i * 5 && mmap->vm_end == i * 5 + 2);
    rbn = rb_node_next(&(mm->mmap_root), rbn);
  }

  for (i = 5; i <= 5 * step2; i += 5) {
    ProcVMA *vma1 = vm_find_vma(mm, i);
    assert(vma1 != NULL);
    ProcVMA *vma2 = vm_find_vma(mm, i+1);
    assert(vma2 != NULL);
    ProcVMA *vma3 = vm_find_vma(mm, i+2);
    assert(vma3 == NULL);
    ProcVMA *vma4 = vm_find_vma(mm, i+3);
    assert(vma4 == NULL);
    ProcVMA *vma5 = vm_find_vma(mm, i+4);
    assert(vma5 == NULL);

    assert(vma1->vm_start == i && vma1->vm_end == i + 2);
    assert(vma2->vm_start == i && vma2->vm_end == i + 2);
  }

  for (i =4; i>=0; i--) {
    ProcVMA *vma_below_5 = vm_find_vma(mm,i);
    assert(vma_below_5 == NULL);
  }

  vm_destroy_proc(mm);
  printf("[vmm] Passes check_vma_struct test.\n");
}

// check correctness of vmm
static void check_vmm(void) {
  check_vma_struct();
  //check_pgfault();

  printf("[vmm] All tests passes, standby ready.\n");
}

void vmm_init(void)
{
  check_vmm();
}

// ---- Page fault handler ----
volatile unsigned int vm_pgfault_count = 0;

/* do_pgfault - interrupt handler to process the page fault execption
 * @mm         : the control struct for a set of vma using the same PDT
 * @error_code : the error code recorded in trapframe->tf_err which is setted by x86 hardware
 * @addr       : the addr which causes a memory access exception, (the contents of the CR2 register)
 *
 * CALL GRAPH: trap--> trap_dispatch-->pgfault_handler-->do_pgfault
 * The processor provides ucore's do_pgfault function with two items of information to aid in diagnosing
 * the exception and recovering from it.
 *   (1) The contents of the CR2 register. The processor loads the CR2 register with the
 *       32-bit linear address that generated the exception. The do_pgfault fun can
 *       use this address to locate the corresponding page directory and page-table
 *       entries.
 *   (2) An error code on the kernel stack. The error code for a page fault has a format different from
 *       that for other exceptions. The error code tells the exception handler three things:
 *         -- The P flag   (bit 0) indicates whether the exception was due to a not-present page (0)
 *            or to either an access rights violation or the use of a reserved bit (1).
 *         -- The W/R flag (bit 1) indicates whether the memory access that caused the exception
 *            was a read (0) or write (1).
 *         -- The U/S flag (bit 2) indicates whether the processor was executing at user mode (1)
 *            or supervisor mode (0) at the time of the exception.
 */
int vm_pgfault_handler(ProcVM *mm, uint32_t error_code, uintptr_t addr)
{
  int ret = -EINVAL;
  // Try to find a vma which include addr.
  ProcVMA *vma = vm_find_vma(mm, addr);
  ++vm_pgfault_count;
  
  //If the addr is in the range of a mm's vma?
  if (vma == NULL || vma->vm_start > addr) {
    printf("[vmm] pgfault_handler: not valid vma 0x%08x\n", addr);
    goto failed;
  }
  
  //check the error_code
  switch (error_code & 3) {
    /* error code flag : default is 3 (W/R=1, P=1): write, present */
    default:
    /* error code flag : (W/R=1, P=0): write, not present */
    case 2:
      if (!(vma->vm_flags & VM_WRITE)) {
        printf("[vmm] pgfault_handler: error code flag = write AND not present,"
               " but the addr's vma cannot write\n");
        goto failed;
      }
      break;
    /* error code flag : (W/R=0, P=1): read, present */
    case 1:
      printf("[vmm] pgfault_handler: error code flag = read AND present.\n");
      goto failed;
    /* error code flag : (W/R=0, P=0): read, not present */
    case 0:
      if (!(vma->vm_flags & (VM_READ | VM_EXEC))) {
        printf("[vmm] pgfault_handler: error code flag = read AND not present,"
               " but the addr's vma cannot read or exec.\n");
        goto failed;
      }
      break;
  }
  
  /* IF (write an existed addr ) OR
   *    (write an non_existed addr && addr is writable) OR
   *    (read  an non_existed addr && addr is readable)
   * THEN
   *    continue process
   */
  uint32_t perm = PTE_U;
  if (vma->vm_flags & VM_WRITE) {
    perm |= PTE_W;
  }
  addr = K_ROUND_DOWN(addr, PGSIZE); // Round to page margin.
  ret = -ENOMEM;
  
  // mm should be associated with particular process, so mm->pgdir here.
  pte_t *ptep = NULL;
  ptep = get_pte(mm->pgdir, addr, 1);
  if (ptep == NULL) {
    printf("[vmm] Cannot create page table entry for address 0x%08x\n", addr);
    goto failed;
  }
  
  // Page table entry does not exist, which indicates that this page is never 
  // created. So just create a new one.
  Page* page;

  if (*ptep == 0) {
    if ((page = pgdir_alloc_page(mm->pgdir, addr, perm)) == NULL) {
      printf("[vmm] Cannot create page for address 0x%08x\n", addr);
      goto failed;
    }
  }
  
  // Otherwise, the page was ever created, but is in swap at the moment.
  // So we need to load data from disk into the memory.
  else {
    if ((*ptep & PTE_P) == 0) {
      panic("[vmm] pgfault_handler: Page seems to be on swap, while swap is "
            "not enabled.");
#if 0
      // Check whether swap has been inited.
      if (!swap_init_ok) {
          cprintf("Swap is never initialized but a page [pte=%08x] on swap is requested.\n", *ptep);
          goto failed;
      }
      
      // Swep in the required page.
      if (swap_in(mm, addr, &page) != 0) {
          cprintf("Cannot swap in page 0x%08x.\n", *ptep);
          goto failed;
      }
      page_insert(mm->pgdir, page, addr, perm);
      
      // Register the new page to vmm manager.
      swap_map_swappable(mm, addr, page, 1);
#endif 
    }
  }
  
  ret = 0;
  
failed:
  return ret;
}
