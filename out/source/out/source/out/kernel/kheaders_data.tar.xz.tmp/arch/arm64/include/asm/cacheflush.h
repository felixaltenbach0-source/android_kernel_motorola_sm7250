
#ifndef __ASM_CACHEFLUSH_H
#define __ASM_CACHEFLUSH_H

#include <linux/kgdb.h>
#include <linux/mm.h>


#define PG_dcache_clean PG_arch_1


extern void flush_cache_all(void);
extern void __flush_icache_range(unsigned long start, unsigned long end);
extern int  invalidate_icache_range(unsigned long start, unsigned long end);
extern void __flush_dcache_area(void *addr, size_t len);
extern void __inval_dcache_area(void *addr, size_t len);
extern void __clean_dcache_area_poc(void *addr, size_t len);
extern void __clean_dcache_area_pop(void *addr, size_t len);
extern void __clean_dcache_area_pou(void *addr, size_t len);
extern long __flush_cache_user_range(unsigned long start, unsigned long end);
extern void sync_icache_aliases(void *kaddr, unsigned long len);

static inline void flush_icache_range(unsigned long start, unsigned long end)
{
	__flush_icache_range(start, end);

	

	
	if (in_dbg_master())
		return;

	kick_all_cpus_sync();
}

static inline void flush_cache_mm(struct mm_struct *mm)
{
}

static inline void flush_cache_page(struct vm_area_struct *vma,
				    unsigned long user_addr, unsigned long pfn)
{
}

static inline void flush_cache_range(struct vm_area_struct *vma,
				     unsigned long start, unsigned long end)
{
}


extern void __dma_map_area(const void *, size_t, int);
extern void __dma_unmap_area(const void *, size_t, int);
extern void __dma_flush_area(const void *, size_t);
extern void __dma_inv_area(const void *start, size_t size);
extern void __dma_clean_area(const void *start, size_t size);

#define dmac_flush_range(start, end) \
	__dma_flush_area(start, (void *)(end) - (void *)(start))
#define dmac_inv_range(start, end) \
	__dma_inv_area(start, (void *)(end) - (void *)(start))
#define dmac_clean_range(start, end) \
	__dma_clean_area(start, (void *)(end) - (void *)(start))


extern void copy_to_user_page(struct vm_area_struct *, struct page *,
	unsigned long, void *, const void *, unsigned long);
#define copy_from_user_page(vma, page, vaddr, dst, src, len) \
	do {							\
		memcpy(dst, src, len);				\
	} while (0)

#define flush_cache_dup_mm(mm) flush_cache_mm(mm)


#define ARCH_IMPLEMENTS_FLUSH_DCACHE_PAGE 1
extern void flush_dcache_page(struct page *);

static inline void __flush_icache_all(void)
{
	if (cpus_have_const_cap(ARM64_HAS_CACHE_DIC))
		return;

	asm("ic	ialluis");
	dsb(ish);
}

#define flush_dcache_mmap_lock(mapping)		do { } while (0)
#define flush_dcache_mmap_unlock(mapping)	do { } while (0)


#define flush_icache_page(vma,page)	do { } while (0)


static inline void flush_cache_vmap(unsigned long start, unsigned long end)
{
}

static inline void flush_cache_vunmap(unsigned long start, unsigned long end)
{
}

int set_memory_valid(unsigned long addr, int numpages, int enable);

#endif
