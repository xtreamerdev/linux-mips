/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994 - 2003 by Ralf Baechle
 */
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>

#include <asm/cacheflush.h>

asmlinkage int sys_cacheflush(void *addr, int bytes, int cache)
{
	/* This should flush more selectivly ...  */
	__flush_cache_all();

	return 0;
}

#if 0

void flush_cache_all(void)
{
	if (!cpu_has_dc_aliases)
		return;

	zap_dcache_all();
	zap_icache_all();
}

void flush_cache_mm(struct mm_struct *mm)
{
	if (!cpu_has_dc_aliases)
		return;

	if (!cpu_context(smp_processor_id(), mm) != 0)
		return;

	zap_dcache_all();
	zap_icache_all();
}

void flush_cache_page(struct vm_area_struct *vma, unsigned long addr)
{
	if (cpu_has_dc_aliases) {
		zap_dcache_page(addr);
	}

	if (vma->vm_flags & VM_EXEC) {
		zap_icache_page(addr);
	}
}

void flush_cache_range(struct mm_struct *mm, unsigned long start,
	unsigned long end)
{
	if (!cpu_has_dc_aliases)
		return;

	zap_dcache_all();
	zap_icache_all();	/* Needed in this cache-op? */
}

void flush_icache_range(unsigned long start, unsigned long end)
{
	/* XXX */
}

void flush_icache_all(void)
{
	if (!cpu_has_vtag_icache)
		return;

	zap_icache_all();
}
#endif

void flush_dcache_page(struct page *page)
{
	unsigned long addr;

	if (!cpu_has_dc_aliases)
		return;

	if (page->mapping && page->mapping->i_mmap == NULL &&
	    page->mapping->i_mmap_shared == NULL) {
		SetPageDcacheDirty(page);

		return;
	}

	/*
	 * We could delay the flush for the !page->mapping case too.  But that
	 * case is for exec env/arg pages and those are 99% certainly going to
	 * get faulted into the tlb (and thus flushed) anyways.
	 */
	addr = (unsigned long) page_address(page);
	flush_data_cache_page(addr);
}

void __update_cache(struct vm_area_struct *vma, unsigned long address,
        pte_t pte)
{
	unsigned long addr;
	struct page *page;

	if (!cpu_has_dc_aliases)
		return;

	page = pte_page(pte);
	if (VALID_PAGE(page) && page->mapping &&
	    (page->flags & (1UL << PG_dcache_dirty))) {
		if (pages_do_alias(page_address(page), address & PAGE_MASK)) {
			addr = (unsigned long) page_address(page);
			flush_data_cache_page(addr);
		}

		ClearPageDcacheDirty(page);
	}
}
