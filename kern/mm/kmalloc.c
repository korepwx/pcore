// @file: kern/mm/kmalloc.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-17
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include <asm/bitops.h>
#include <pcore/list.h>
#include <pcore/pmm.h>
#include <pcore/sync.h>

// Configuration of the slub algorithm.
#ifndef PAGE_SIZE
# define PAGE_SIZE PGSIZE
#endif
#ifndef PAGE_SHIFT
# define PAGE_SHIFT 12
#endif

#define KMALLOC_MIN_SIZE 8 // The minimum object size is 8B.
#define KMALLOC_MIN_SIZE_ORDER 3 // The minimum object size is 8B.

#define SLUB_MAX_ORDER (18) // The maximum size for a slab is 256K (excluded).
#define SLUB_PAGE_SHIFT (SLUB_MAX_ORDER + 1) // Object size ranges from 1 to 256K (excluded).
#define SLUB_MAX_SIZE (1 << SLUB_MAX_ORDER)
#define SLUB_ALIGN_SIZE 8

// find last set bit in word
// Get 2^(n-1) < X <= 2^n
static inline uint32_t slub_get_order(size_t n) {
  return (n > 1) ? ufls(n - 1) : 0;
}


/********************************************************************
 *     Slub algorithm interfaces
 ********************************************************************/
typedef unsigned int gfp_t;
typedef unsigned int kmem_bufctl_t;

struct slab_s {
  KListEntry slab_link;   // Link to next slab in list.
  void *free_objects;     // The address of first object (kernel virtual addr).
                          // Since the size of an object is larger than 8 bytes,
                          // I just put the next free object address in prev.
  void *mem_base;         // The start address of objects.
  size_t inuse;           // Number of objects in use.

  unsigned int pad[3];    // Pad slab_t to 32 bytes.
};
typedef struct slab_s slab_t;

#define le2slab(le, member) \
  to_struct((le), slab_t, member)

struct kmem_cache_s {
  int objsize;  // The size of an object excluding meta data.
  int padsize;  // The size of memory allocated.
  int num;      // Number of objects in a slab.

  KListEntry slabs_full;      // List of full slab caches.
  KListEntry slabs_partial;   // List of partial slab caches.
  unsigned long nr_partial;   // Number of partial slab caches.

  size_t page_order;  // order of page per slab.
};
typedef struct kmem_cache_s kmem_cache_t;

// Slub algorithm kernel memory.
kmem_cache_t slab_caches[SLUB_PAGE_SHIFT];

// initialize the slub algorithm.
static void kmem_cache_init(kmem_cache_t *s, size_t objsize, size_t align);
static kmem_cache_t* kmalloc_get_cache(size_t order) ;
static void check_slub(void);

void slab_init(void) 
{
  size_t i = 0;
  size_t size = 1;

  for (; i<SLUB_MAX_ORDER; ++i, size <<= 1) {
    kmem_cache_init(kmalloc_get_cache(i), size, SLUB_ALIGN_SIZE);
  }

  printf("[pmm] Select slub as kernel memory allocator.\n");
  check_slub();
}

/********************************************************************
 *     Slub algorithm details.
 ********************************************************************/

// Get the correct slab corresponding to size.
static inline 
kmem_cache_t* kmalloc_get_cache(size_t order) 
{
  return &slab_caches[order];
}

// Calculate the order and other information of slab.
static inline 
void estimate_slab_order(kmem_cache_t *s, size_t objsize, size_t align) 
{
  size_t order, slabsize = 1 << PAGE_SHIFT;
  kassert(objsize > 0 && objsize < SLUB_MAX_SIZE);

  for (order=0; order<=SLUB_MAX_ORDER; ++order, slabsize <<= 1) {
    size_t num, remain;

    // estimate the number of objects in a slab.
    num = (slabsize - sizeof(slab_t)) / objsize;
    remain = slabsize - sizeof(slab_t) - num * objsize;

    // Waste at most 128B memory.
    if (remain * 8 <= slabsize) { 
      s->objsize = objsize;
      s->padsize = objsize;   // TODO: use padsize instead of objsize.
      s->num = num;
      s->page_order = order;
      return;
    }
  }

  kpanic("[SLUB] Cannot determine order for kmem_cache (objsize=%d).", 
         (int)objsize);
}

// Initialize the slab cache item.
static inline 
void kmem_cache_init(kmem_cache_t *s, size_t objsize, size_t align) 
{
  kassert(sizeof(slab_t) == 32);

  // Initialize slab list.
  klist_init(&(s->slabs_full));
  klist_init(&(s->slabs_partial));
  s->nr_partial = 0;

  // Round up to align for performance.
  objsize = K_ROUND_UP(objsize, align);

  // Set the initial fields of kmem_cache.
  estimate_slab_order(s, objsize, align);
}

// Associate page with kmem_cache_t & slab_t
static inline 
void slab_page_init(Page* page, kmem_cache_t *s, slab_t *slab)
{
  kmem_cache_t **cachep = (kmem_cache_t**)&(page->page_link.prev);
  slab_t **slabp = (slab_t**)&(page->page_link.next);
  *cachep = s;
  *slabp = slab;
}

// Allocate a new slab for kmem_cache.
static inline
bool kmem_cache_grow(kmem_cache_t *s)
{
  // Allocate enough room for the slab.
  size_t order_size = 1 << s->page_order;
  Page *first_page = kalloc_pages(order_size);
  if (first_page == NULL) {
    goto failed;
  }

  // The first page in slab should carry slab_t as header.
  Page* page = first_page;
  slab_t *slabp = (slab_t*)page2kva(page);

  // So then I associate each page with kmem_cache_t & slab_t.
  do {
    slab_page_init(page, s, slabp);
    PageSetFlag(page, slab);
    ++page;
  } while (--order_size);

  // Link all free objects.
  size_t i;
  void *mem = page2kva(first_page) + sizeof(slab_t);
  slabp->free_objects = slabp->mem_base = mem;
  for (i=0; i<s->num-1; ++i) {
    *(void**)mem = mem + s->objsize;
    mem += s->objsize;
  }
  *(void**)mem = 0;

  // Add slab into list.
  bool intr_flag;
  local_intr_save(intr_flag);
  {
    // TODO: use RCU lock to guard list modify.
    klist_add(&(s->slabs_partial), &(slabp->slab_link));
  }
  local_intr_restore(intr_flag);
  return 1;

failed:
  return 0;
}

// Destroy a slab in kmem_cache.
static inline
void kmalloc_cache_destroy(kmem_cache_t *s, slab_t *slab) 
{
  Page* page = kva2page(slab->mem_base - sizeof(slab_t));
  Page* p = page;
  size_t order_size = s->page_order;

  do {
    kassert(PageHasFlag(p, slab));
    PageClearFlag(p, slab);
    ++p;
  } while (--order_size);

  kfree_pages(page, order_size);
}

// Allocate an object from specified slab.
static inline
void* kmalloc_cache_alloc_one(kmem_cache_t *s, slab_t *slab) 
{
  kassert(slab->free_objects != NULL);
  ++slab->inuse;
  void **mem = slab->free_objects;
  slab->free_objects = *mem;
  if (slab->free_objects == NULL) {
    klist_remove(&(slab->slab_link));
    klist_add(&(s->slabs_full), &(slab->slab_link));
  }
  return mem;
}

// Allocate an object from kmem_cache. Will create a new slab if needed.
static inline
void* kmalloc_cache_alloc(kmem_cache_t* s)
{
  void *objp;
  bool intr_flag;

alloc_obj:
  local_intr_save(intr_flag);
  if (klist_empty(&(s->slabs_partial))) {
    goto alloc_slab;
  }
  slab_t *slabp = le2slab(klist_next(&(s->slabs_partial)), slab_link);
  objp = kmalloc_cache_alloc_one(s, slabp);
  local_intr_restore(intr_flag);
  return objp;

alloc_slab:
  local_intr_restore(intr_flag);
  if (kmem_cache_grow(s)) {
    goto alloc_obj;
  }
  return NULL;
}

// Free one object from specified slab.
static inline
void kmalloc_cache_free_one(kmem_cache_t *s, slab_t *slab, void *objp) 
{
  // Let object be the first item of free_objects list.
  *(void**)objp = slab->free_objects;
  slab->free_objects = objp;
  --slab->inuse;

  // Whether we should delete the slab, or put it in partial list?
  if (slab->inuse == 0) {
    klist_remove(&(slab->slab_link));
    kmalloc_cache_destroy(s, slab);
  } else if (slab->inuse == s->num - 1) {
    klist_remove(&(slab->slab_link));
    klist_add(&(s->slabs_partial), &(slab->slab_link));
  }
}

// Free one object, and alter the slab if needed.
#define GET_PAGE_CACHE(page)                                \
    (kmem_cache_t *)((page)->page_link.next)

#define GET_PAGE_SLAB(page)                                 \
    (slab_t *)((page)->page_link.prev)


// Malloc large objects that are bigger than SLUB_MAX_SIZE.
static inline 
void* kmalloc_large(size_t size, gfp_t flags) 
{
  size_t page_count = (size + PGSIZE - 1) / PGSIZE;
  Page *page = kalloc_pages(page_count);
  PageSetFlag(page, extraslab);
  PageSetFlag(page, property);
  page->property = page_count;
  return page2kva(page);
}

// Free large objects that are bigger than SLUB_MAX_SIZE
static inline 
void kfree_large(void* p) {
  Page *page = kva2page(p);
  size_t page_count = page->property;
  assert(PageHasFlag(page, property));
  PageClearFlag(page, extraslab);
  PageClearFlag(page, property);
  kfree_pages(page, page_count);
}

// Check Slub System.
void check_slub(void) {
  printf("[pmm] Kmalloc passes check_slub test.\n");
}


/********************************************************************
 *     Implementation of kmalloc, kfree & kmalloc_init.
 ********************************************************************/

// Initialize the malloc system.
inline void 
kmalloc_init(void) {
  slab_init();
}

// Malloc object
void* __kmalloc(size_t size, gfp_t flags) 
{
  kmem_cache_t* s;
  void* ret;

  if (size < KMALLOC_MIN_SIZE) {
    size = KMALLOC_MIN_SIZE;
  }
  size = K_ROUND_UP(size, SLUB_ALIGN_SIZE);

  if (size >= SLUB_MAX_SIZE)
    return kmalloc_large(size, flags);
  
  size_t order = slub_get_order(size);
  if ((s = kmalloc_get_cache(order)) == NULL)
    return NULL;

  ret = kmalloc_cache_alloc(s);
  return ret;
}

void* kmalloc(size_t size) 
{
  void* p = __kmalloc(size, 0);
  return p;
}

// Free Object
void kfree(void* objp) 
{
  bool intr_flag;
  if (objp == NULL)
    return; // Do nothing if p is NULL.

  Page* page = kva2page(objp);

  if (PageHasFlag(page, extraslab)) {
    return kfree_large(objp);
  }
  if (!PageHasFlag(page, slab)) {
    kpanic("Try to free %08zx, which is not managed by slub.", (size_t)objp);
  }

  local_intr_save(intr_flag);
  {
    kmalloc_cache_free_one(GET_PAGE_CACHE(page), GET_PAGE_SLAB(page), objp);
  }
  local_intr_restore(intr_flag);
}
