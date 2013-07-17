// @file: arch/i386/mm/pmm.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-12
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

// @note: this file is derived from ucore pmm.c

#include <stdio.h>
#include <pcore/compiler.h>
#include <pcore/pmm.h>
#include <pcore/kmalloc.h>
#include <sort.h>
#include <string.h>
#include <errno.h>
#include <asm/reg.h>

// Global status variables.
size_t pmm_npage;   // page count.
Page *pmm_pages;    // First page address.


// Task State Segment:
static struct taskstate ts = {0};

/* *
 * write_esp0 - change the ESP0 in default task state segment,
 * so that we can use different kernel stack when we trap frame
 * user to kernel.
 * */
void write_esp0(uintptr_t esp0) {
  ts.ts_esp0 = esp0;
}

// virtual address of boot-time page directory
pde_t *pmm_boot_pgdir = NULL;

// physical address of boot-time page directory
uintptr_t pmm_boot_cr3;

/* *
 * The page directory entry corresponding to the virtual address range
 * [VPT, VPT + PTSIZE) points to the page directory itself. Thus, the page
 * directory is treated as a page table as well as a page directory.
 *
 * One result of treating the page directory as a page table is that all PTEs
 * can be accessed though a "virtual page table" at virtual address VPT. And the
 * PTE for number n is stored in pmm_vpt[n].
 *
 * A second consequence is that the contents of the current page directory will
 * always available at virtual address PGADDR(PDX(VPT), PDX(VPT), 0), to which
 * pmm_vpd is set bellow.
 * */
pte_t* const pmm_vpt = (pte_t *)VPT;
pde_t* const pmm_vpd = (pde_t *)PGADDR(PDX(VPT), PDX(VPT), 0);

/* *
 * Global Descriptor Table:
 *
 * The kernel and user segments are identical (except for the DPL). To load
 * the %ss register, the CPL must equal the DPL. Thus, we must duplicate the
 * segments for the user and the kernel. Defined as follows:
 *   - 0x0 :  unused (always faults -- for trapping NULL far pointers)
 *   - 0x8 :  kernel code segment
 *   - 0x10:  kernel data segment
 *   - 0x18:  user code segment
 *   - 0x20:  user data segment
 *   - 0x28:  defined for tss, initialized in gdt_init
 * */
static struct segdesc gdt[] = {
  SEG_NULL,
  [SEG_KTEXT] = SEG(STA_X | STA_R, 0x0, 0xFFFFFFFF, DPL_KERNEL),
  [SEG_KDATA] = SEG(STA_W, 0x0, 0xFFFFFFFF, DPL_KERNEL),
  [SEG_UTEXT] = SEG(STA_X | STA_R, 0x0, 0xFFFFFFFF, DPL_USER),
  [SEG_UDATA] = SEG(STA_W, 0x0, 0xFFFFFFFF, DPL_USER),
  [SEG_TSS]   = SEG_NULL,
};

static struct lgdtdesc gdt_pd = {
  sizeof(gdt) - 1, (uintptr_t)gdt
};

// ---- Common page table manipulate ----

// invalidate a TLB entry, but only if the page tables being
// edited are the ones currently in use by the processor.
void tlb_invalidate(pde_t *pgdir, uintptr_t la) {
  if (read_cr3() == PADDR(pgdir)) {
    invlpg((void *)la);
  }
}

//get_pte - get pte and return the kernel virtual address of this pte for la
//        - if the PT contians this pte didn't exist, alloc a page for PT
// parameter:
//  pgdir:  the kernel virtual base address of PDT
//  la:     the linear address need to map
//  create: a logical value to decide if alloc a page for PT
// return vaule: the kernel virtual address of this pte
pte_t *get_pte(pde_t *pgdir, uintptr_t la, bool create) 
{
  // According to pmm.c:boot_map_segment, the function of get_pte only involves 
  // getting the memory unit of pte. Assign la to pte is not its responsibility.
  pde_t *pde = &pgdir[PDX(la)];
  
  if (*pde == 0) {
    // Return NULL if creation is disabled.
    if (!create) {
      return NULL;
    }
    
    // A new page should be allocated to fill in *pde.
    Page* page = kalloc_pages(1);
    if (page == NULL) return NULL;
    set_page_ref(page, 1);
    
    // Get page physical address.
    uintptr_t pa = page2pa(page);
    memset(KADDR(pa), 0, PGSIZE);
    
    // Assign pte to pde.
    *pde = pa | PTE_P | PTE_W | PTE_U;
  }
  
  return &((pde_t*)
    KADDR(
      PDE_ADDR(*pde)          // Remove the flags along with PTE address.
    )                         // Get the virtual address of PTE page.
  )[PTX(la)];                 // Access the PTE entity.
}

//get_page - get related Page struct for linear address la using PDT pgdir
Page *get_page(pde_t *pgdir, uintptr_t la, pte_t **ptep_store)
{
  pte_t *ptep = get_pte(pgdir, la, 0);
  if (ptep_store != NULL) {
    *ptep_store = ptep;
  }
  if (ptep != NULL && *ptep & PTE_P) {
    return pa2page(*ptep);
  }
  return NULL;
}

//page_remove_pte - free an Page sturct which is related linear address la
//                - and clean(invalidate) pte which is related linear address la
//note: PT is changed, so the TLB need to be invalidate 
static inline void
page_remove_pte(pde_t *pgdir, uintptr_t la, pte_t *ptep) {
  Page* page;
  
  // Validate the ptep and get page address.
  if (ptep == NULL || !(*ptep & PTE_P)) { 
    return;
  }
  page = pte2page(*ptep);
  
  // de-reference the page and free if necessary.
  if (page_ref_dec(page) == 0) {
    kfree_page(page);
  }
  
  // Cleanup.
  *ptep = 0;
  tlb_invalidate(pgdir, la);
}

//page_remove - free an Page which is related linear address la and has an validated pte
void page_remove(pde_t *pgdir, uintptr_t la) {
  pte_t *ptep = get_pte(pgdir, la, 0);
  if (ptep != NULL) {
    page_remove_pte(pgdir, la, ptep);
  }
}

//page_insert - build the map of phy addr of an Page with the linear addr la
// paramemters:
//  pgdir: the kernel virtual base address of PDT
//  page:  the Page which need to map
//  la:    the linear address need to map
//  perm:  the permission of this Page which is setted in related pte
// return value: always 0
//note: PT is changed, so the TLB need to be invalidate 
int page_insert(pde_t *pgdir, Page *page, uintptr_t la, uint32_t perm) {
  pte_t *ptep = get_pte(pgdir, la, 1);
  if (ptep == NULL) {
    return -ENOMEM;
  }
  page_ref_inc(page);
  if (*ptep & PTE_P) {
    Page *p = pte2page(*ptep);
    if (p == page) {
      page_ref_dec(page);
    }
    else {
      page_remove_pte(pgdir, la, ptep);
    }
  }
  *ptep = page2pa(page) | PTE_P | perm;
  tlb_invalidate(pgdir, la);
  return 0;
}

// ---- Following utilites are used for pm_manager_init ----
static PMManager pmm;
static void pm_manager_init()
{
  // select buddy pmm instance.
  extern PMManager pmm_buddy_instance;
  memcpy(&pmm, &pmm_buddy_instance, sizeof(pmm));
  
  // Initialize the pmm instance.
  KCALL(&pmm, init, ());
}

// ---- Following utilites are used for page_init ----

// Add physical memory into manager.
static void insert_free_mem(Page *base, size_t n)
{
  KCALL(&pmm, insert_free_mem, (base, n));
}

// Memory bound information.
typedef struct {
  uint64_t addr;
  int begin;  // is begin address.
  int avail;  // Is available memory.
} MemoryBound;

static int memory_bound_compare(const void* a, const void* b) {
  uint64_t off = ((MemoryBound*)b)->addr - ((MemoryBound*)a)->addr;
  if (off > 0) 
    return -1;
  else if (off < 0) 
    return 1;
  return 0;
}

// Detect physical memory and build up page link.
static void page_init()
{
  E820Map *memmap = (E820Map*)(0x8000 + KERNBASE);
  uint64_t pmemsize = 0;
  
  // Gather all memory bounds for later process.
  MemoryBound bounds[E820MAX * 2];
  int boundlen = 0;
  
#define PUSH_BOUND(ADDR, IS_BEGIN, IS_AVAIL)        \
  bounds[boundlen].addr = (ADDR);                   \
  bounds[boundlen].begin = (IS_BEGIN);              \
  bounds[boundlen].avail = (IS_AVAIL);              \
  ++boundlen;
  
  // Determine the maximum address of memory.
  printf("[pmm] Detecting available memory ...\n");
  int i;
  for (i=0; i<memmap->nr_map; ++i) {
    uint64_t beg = memmap->map[i].addr, end = memmap->map[i].size + beg;
    if (memmap->map[i].type == E820_ARM) {
      if (pmemsize < end && beg < KMEMSIZE) {
        pmemsize = end;
      }
    }
    // push into bound array
    PUSH_BOUND(beg, 1, memmap->map[i].type == E820_ARM);
    PUSH_BOUND(end, 0, memmap->map[i].type == E820_ARM);
    // dump e820map.
    printf("  e820map range [0x%010jx, 0x%010jx], type %d\n", 
           beg, end, memmap->map[i].type);
  }
  if (pmemsize > KMEMSIZE)
    pmemsize = KMEMSIZE;
  
  // The memory just after kernel ELF image is used for page table.
  extern char end[];
  pmm_npage = (size_t)(pmemsize / PGSIZE);
  pmm_pages = (Page*)K_ROUND_UP((uintptr_t)end, PGSIZE);
  
  for (i=0; i<pmm_npage; ++i) {
    PageSetFlag(pmm_pages+i, reserved);
  }
  
  // Determine the minimum free memory.
  uintptr_t freemem = 
    PADDR((uintptr_t) pmm_pages + sizeof(Page) * pmm_npage);
  
  // Gather all free memories into physical memory manager.
  insertion_sort
    (bounds, karraysize(bounds), sizeof(bounds[0]), memory_bound_compare);
    
  int open[2] = {0, 0};  // open[0]: reserved, open[1]: freemem.
  uint64_t free_beg = 0, free_end = 0;
  
  for (i=0; i<boundlen; ++i) {
    uint64_t addr = bounds[i].addr;
    int begin = bounds[i].begin;
    int avail = bounds[i].avail;
    
    // Begin bound of memory chunk.
    if (begin) {
      ++open[avail];
      
      // Perform differently on just-open range.
      switch (avail) {
        // reserved
        case 0:
          if (open[0] == 1 && open[1] > 0) {
            free_end = addr;
            goto insert_mem;
          }
          break;
        
        // freemem
        case 1:
          if (open[0] == 0 && open[1] == 1) {
            free_beg = addr;
            goto insert_mem;
          }
          break;
      };
    }
    
    // End bound of memory chunk.
    else {
      // It's not necessary to check overflow here.  See the first loop.
      --open[avail];
      
      // Perform differently on just-closed range.
      switch (avail) {
        // reserved.
        case 0:
          if (open[0] == 0 && open[1] > 0) {
            free_beg = addr;
            goto insert_mem;
          }
          break;
        
        // freemem.
        case 1:
          if (open[0] == 0 && open[1] == 0) {
            free_end = addr;
            goto insert_mem;
          }
          break;
      };
    }  // if (begin)
    continue;
    
insert_mem:
    // avoid using kernel & device memory space.
    if (free_beg < freemem)
      free_beg = freemem;
    if (free_end > pmemsize) 
      free_end = pmemsize;
    
    // align to page bound.
    free_beg = K_ROUND_UP(free_beg, PGSIZE);
    free_end = K_ROUND_DOWN(free_end, PGSIZE);
    
    if (free_beg < free_end) {
      printf("[pmm] Found free memory [0x%010jx, 0x%010jx]\n", free_beg, free_end);
      insert_free_mem(pa2page(free_beg), (free_end - free_beg) / PGSIZE);
    }
  }  // for (bounds)
}

// ---- Following utilites are used for pm_manager_test ----
static void pm_manager_test()
{
  KCALL(&pmm, check, ());
  printf("[pmm] PMManager '%s' passes page_alloc test.\n", pmm.name);
}

// ---- Following utilities are used for pgtable_init ----
//      Codes are derived from ucore

//boot_alloc_page - allocate one page using pmm->alloc_pages(1) 
static void* boot_alloc_page(void) {
  Page *p = kalloc_page();
  if (p == NULL) {
    kpanic("boot_alloc_page failed.\n");
  }
  return page2kva(p);
}

//boot_map_segment - setup&enable the paging mechanism
// parameters
//  la:   linear address of this memory need to map (after x86 segment map)
//  size: memory size
//  pa:   physical address of this memory
//  perm: permission of this memory  
static void boot_map_segment
  (pde_t *pgdir, uintptr_t la, size_t size, uintptr_t pa, uint32_t perm) 
{
  kassert(PGOFF(la) == PGOFF(pa));
  size_t n = K_ROUND_UP(size + PGOFF(la), PGSIZE) / PGSIZE;
  la = K_ROUND_DOWN(la, PGSIZE);
  pa = K_ROUND_DOWN(pa, PGSIZE);
  for (; n > 0; n --, la += PGSIZE, pa += PGSIZE) {
    pte_t *ptep = get_pte(pgdir, la, 1);
    kassert(ptep != NULL);
    *ptep = pa | PTE_P | perm;
  }
}

static void enable_paging(void) {
  write_cr3(pmm_boot_cr3);

  // turn on paging
  uint32_t cr0 = read_cr0();
  cr0 |= CR0_PE | CR0_PG | CR0_AM | CR0_WP | CR0_NE | CR0_TS | CR0_EM | CR0_MP;
  cr0 &= ~(CR0_TS | CR0_EM);
  write_cr0(cr0);
}

/* gdt_init - initialize the default GDT and TSS */
static void gdt_init(void) {
  // set boot kernel stack and default SS0
  write_esp0((uintptr_t)bootstacktop);
  ts.ts_ss0 = KERNEL_DS;

  // initialize the TSS filed of the gdt
  gdt[SEG_TSS] = SEGTSS(STS_T32A, (uintptr_t)&ts, sizeof(ts), DPL_KERNEL);

  // reload all segment registers
  lgdt(&gdt_pd);

  // load the TSS
  ltr(GD_TSS);
}

// Check the function of page table.
static void check_pgdir(void) {
  kassert(pmm_npage <= KMEMSIZE / PGSIZE);
  kassert(pmm_boot_pgdir != NULL && (uint32_t)PGOFF(pmm_boot_pgdir) == 0);
  kassert(get_page(pmm_boot_pgdir, 0x0, NULL) == NULL);

  Page *p1, *p2;
  p1 = kalloc_page();
  kassert(page_insert(pmm_boot_pgdir, p1, 0x0, 0) == 0);

  pte_t *ptep;
  kassert((ptep = get_pte(pmm_boot_pgdir, 0x0, 0)) != NULL);
  kassert(pa2page(*ptep) == p1);
  kassert(page_ref(p1) == 1);

  ptep = &((pte_t *)KADDR(PDE_ADDR(pmm_boot_pgdir[0])))[1];
  kassert(get_pte(pmm_boot_pgdir, PGSIZE, 0) == ptep);

  p2 = kalloc_page(); 
  kassert(page_insert(pmm_boot_pgdir, p2, PGSIZE, PTE_U | PTE_W) == 0);
  kassert((ptep = get_pte(pmm_boot_pgdir, PGSIZE, 0)) != NULL);
  kassert(*ptep & PTE_U);
  kassert(*ptep & PTE_W);
  kassert(pmm_boot_pgdir[0] & PTE_U);
  kassert(page_ref(p2) == 1);

  kassert(page_insert(pmm_boot_pgdir, p1, PGSIZE, 0) == 0);
  kassert(page_ref(p1) == 2);
  kassert(page_ref(p2) == 0);
  kassert((ptep = get_pte(pmm_boot_pgdir, PGSIZE, 0)) != NULL);
  kassert(pa2page(*ptep) == p1);
  kassert((*ptep & PTE_U) == 0);

  page_remove(pmm_boot_pgdir, 0x0);
  kassert(page_ref(p1) == 1);
  kassert(page_ref(p2) == 0);

  page_remove(pmm_boot_pgdir, PGSIZE);
  kassert(page_ref(p1) == 0);
  kassert(page_ref(p2) == 0);

  kassert(page_ref(pa2page(pmm_boot_pgdir[0])) == 1);
  kfree_page(pa2page(pmm_boot_pgdir[0]));
  pmm_boot_pgdir[0] = 0;

  printf("[pmm] Page table passes check_pgdir test.\n");
}

static void check_boot_pgdir(void) {
  pte_t *ptep;
  int i;
  for (i = 0; i < pmm_npage; i += PGSIZE) {
    kassert((ptep = get_pte(pmm_boot_pgdir, (uintptr_t)KADDR(i), 0)) != NULL);
    kassert(PTE_ADDR(*ptep) == i);
  }

  kassert(PDE_ADDR(pmm_boot_pgdir[PDX(VPT)]) == PADDR(pmm_boot_pgdir));

  kassert(pmm_boot_pgdir[0] == 0);

  Page *p;
  p = kalloc_page();
  kassert(page_insert(pmm_boot_pgdir, p, 0x100, PTE_W) == 0);
  kassert(page_ref(p) == 1);
  kassert(page_insert(pmm_boot_pgdir, p, 0x100 + PGSIZE, PTE_W) == 0);
  kassert(page_ref(p) == 2);

  const char *str = "pcore: Hello world!!";
  strcpy((void *)0x100, str);
  kassert(strcmp((void *)0x100, (void *)(0x100 + PGSIZE)) == 0);

  *(char *)(page2kva(p) + 0x100) = '\0';
  kassert(strlen((const char *)0x100) == 0);

  kfree_page(p);
  kfree_page(pa2page(PDE_ADDR(pmm_boot_pgdir[0])));
  pmm_boot_pgdir[0] = 0;

  printf("[pmm] Page table passes check_boot_pgdir test.\n");
}

// initialize the page table.
// Codes are derived from ucore.
static void inline pgtable_init(void) 
{
  // create pmm_boot_pgdir, an initial page directory(Page Directory Table, PDT)
  pmm_boot_pgdir = boot_alloc_page();  // The address of the page itself (not pde or pte).
  memset(pmm_boot_pgdir, 0, PGSIZE);
  pmm_boot_cr3 = PADDR(pmm_boot_pgdir);
  
  check_pgdir();
  
  // recursively insert pmm_boot_pgdir in itself
  // to form a virtual page table at virtual address VPT
  
  // VPT is the virtual address of page directory.
  // Here use self-mapping to loopback the page index of directory.
  pmm_boot_pgdir[PDX(VPT)] = PADDR(pmm_boot_pgdir) | PTE_P | PTE_W; 
  
  // map all physical memory to linear memory with base linear addr KERNBASE
  //linear_addr KERNBASE~KERNBASE+KMEMSIZE = phy_addr 0~KMEMSIZE
  //But shouldn't use this map until enable_paging() & gdt_init() finished.
  boot_map_segment(pmm_boot_pgdir, KERNBASE, KMEMSIZE, 0, PTE_W);
  
  //temporary map: 
  //virtual_addr 3G~3G+4M = linear_addr 0~4M = linear_addr 3G~3G+4M = phy_addr 0~4M    
  
  // Now the kernel address is KERNBASE + offset. After segment selector, it 
  // becomes 0 + offset. To fulfill the require, let page[0]->page[KERNBASE].
  pmm_boot_pgdir[0] = pmm_boot_pgdir[PDX(KERNBASE)];
  
  enable_paging();
  
  //reload gdt(third time,the last time) to map all physical memory
  //virtual_addr 0~4G=liear_addr 0~4G
  //then set kernel stack(ss:esp) in TSS, setup TSS in gdt, load TSS
  gdt_init();
  
  //disable the map of virtual_addr 0~4M
  pmm_boot_pgdir[0] = 0;
  
  //now the basic virtual memory map(see memalyout.h) is established.
  //check the correctness of the basic virtual memory map.
  check_boot_pgdir();
}

// ---- Following codes are used for print_pgdir ----
//get_pgtable_items - In [left, right] range of PDT or PT, find a continuous linear addr space
//                  - (left_store*X_SIZE~right_store*X_SIZE) for PDT or PT
//                  - X_SIZE=PTSIZE=4M, if PDT; X_SIZE=PGSIZE=4K, if PT
// paramemters:
//  left:        no use ???
//  right:       the high side of table's range
//  start:       the low side of table's range
//  table:       the beginning addr of table
//  left_store:  the pointer of the high side of table's next range
//  right_store: the pointer of the low side of table's next range
// return value: 0 - not a invalid item range, perm - a valid item range with perm permission 
static int get_pgtable_items(size_t left, size_t right, size_t start, 
                             uintptr_t *table, size_t *left_store, 
                             size_t *right_store)
{
  if (start >= right) {
    return 0;
  }
  while (start < right && !(table[start] & PTE_P)) {
    start ++;
  }
  if (start < right) {
    if (left_store != NULL) {
      *left_store = start;
    }
    int perm = (table[start ++] & PTE_USER);
    while (start < right && (table[start] & PTE_USER) == perm) {
      start ++;
    }
    if (right_store != NULL) {
      *right_store = start;
    }
    return perm;
  }
  return 0;
}

//perm2str - use string 'u,r,w,-' to present the permission
static const char* perm2str(int perm) {
  static char str[4];
  str[0] = (perm & PTE_U) ? 'u' : '-';
  str[1] = 'r';
  str[2] = (perm & PTE_W) ? 'w' : '-';
  str[3] = '\0';
  return str;
}

void print_pgdir(void) {
  printf("-------------------- BEGIN --------------------\n");
  size_t left, right = 0, perm;
  while ((perm = get_pgtable_items(
            0, NPDEENTRY, right, pmm_vpd, &left, &right)) != 0) {
    printf("PDE(%03x) %08x-%08x %08x %s\n", right - left, left * PTSIZE, 
           right * PTSIZE, (right - left) * PTSIZE, perm2str(perm));
    size_t l, r = left * NPTEENTRY;
    while ((perm = get_pgtable_items(
              left * NPTEENTRY, right * NPTEENTRY, r, pmm_vpt, &l, &r)) != 0) {
      printf("  |-- PTE(%05x) %08x-%08x %08x %s\n", r - l,
             l * PGSIZE, r * PGSIZE, (r - l) * PGSIZE, perm2str(perm));
    }
  }
  printf("--------------------- END ---------------------\n");
}

// ---- Routine to initialize the physical memory manager ----
int pmm_init()
{
  // Check the kernel compiler environment.
  kstatic_assert(KERNBASE % PTSIZE == 0 && KERNTOP % PTSIZE == 0);
  
  // Initialize the pm manager instance.
  pm_manager_init();
  
  // detect physical memory space, and reserve used memory.
  page_init();
  
  // Test whether the pmm manager has successfully inited.
  pm_manager_test();
  
  // Initialize the paging facility
  pgtable_init();
  
  // Print the page table.
  print_pgdir();
  
  // Initialize kmalloc.
  kmalloc_init();
  
  return 0;
}

// ---- Wrappers for PMManager ----
Page* kalloc_pages(size_t n) {
  return KCALL(&pmm, alloc_pages, (n));
}

void kfree_pages(Page* base, size_t n) {
  return KCALL(&pmm, free_pages, (base, n));
}

size_t kfree_page_count(void) {
  return KCALL(&pmm, nr_free_pages, ());
}
