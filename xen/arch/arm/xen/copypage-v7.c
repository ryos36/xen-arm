/*
 *  linux/arch/arm/mm/copypage-v6.c
 *
 *  Copyright (C) 2002 Deep Blue Solutions Ltd, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <xen/spinlock.h>
#include <asm/page.h>

#define DECLARE_CPU_OP(gop, nop)	\
		typeof (nop) gop		\
	__attribute__((weak, alias(#nop)))


#if SHMLBA > 16384
#error FIX ME
#endif

#define from_address	(0xffff8000)
#define to_address	(0xffffc000)

static DEFINE_SPINLOCK(v6_lock);

/*
 * Copy the user page.  No aliasing to deal with so we can just
 * attack the kernel's existing mapping of these pages.
 */
static void v6_copy_user_page_nonaliasing(void *kto, const void *kfrom, unsigned long vaddr)
{
	copy_page(kto, kfrom);
}

DECLARE_CPU_OP(cpu_copy_page, v6_copy_user_page_nonaliasing);

/*
 * Clear the user page.  No aliasing to deal with so we can just
 * attack the kernel's existing mapping of this page.
 */
static void v6_clear_user_page_nonaliasing(void *kaddr, unsigned long vaddr)
{
	clear_page(kaddr);
}

DECLARE_CPU_OP(cpu_clear_page, v6_clear_user_page_nonaliasing);

/*
 * Copy the page, taking account of the cache colour.
 */
#define	SHMLBA	(4 * PAGE_SIZE)		 /* attach addr a multiple of this */

#define CACHE_COLOUR(vaddr)	((vaddr & (SHMLBA - 1)) >> PAGE_SHIFT)
static void v6_copy_user_page_aliasing(void *kto, const void *kfrom, unsigned long vaddr)
{
#if 0
	unsigned int offset = CACHE_COLOUR(vaddr);
	unsigned long from, to;
	struct page *page = virt_to_page(kfrom);

	if (test_and_clear_bit(PG_dcache_dirty, &page->flags))
		__flush_dcache_page(page_mapping(page), page);

	/*
	 * Discard data in the kernel mapping for the new page.
	 * FIXME: needs this MCRR to be supported.
	 */
	__asm__("mcrr	p15, 0, %1, %0, c6	@ 0xec401f06"
	   :
	   : "r" (kto),
	     "r" ((unsigned long)kto + PAGE_SIZE - L1_CACHE_BYTES)
	   : "cc");
#endif

	/*
	 * Now copy the page using the same cache colour as the
	 * pages ultimate destination.
	 */
	spin_lock(&v6_lock);

#if 0
	set_pte_ext(TOP_PTE(from_address) + offset, pfn_pte(__pa(kfrom) >> PAGE_SHIFT, PAGE_KERNEL), 0);
	set_pte_ext(TOP_PTE(to_address) + offset, pfn_pte(__pa(kto) >> PAGE_SHIFT, PAGE_KERNEL), 0);

	from = from_address + (offset << PAGE_SHIFT);
	to   = to_address + (offset << PAGE_SHIFT);

	flush_tlb_kernel_page(from);
	flush_tlb_kernel_page(to);
#endif

	cpu_flush_tlb_all();

	copy_page(kto, kfrom);

	spin_unlock(&v6_lock);
}
//DECLARE_CPU_OP(cpu_copy_page, v6_copy_user_page_aliasing);

/*
 * Clear the user page.  We need to deal with the aliasing issues,
 * so remap the kernel page into the same cache colour as the user
 * page.
 */
static void v6_clear_user_page_aliasing(void *kaddr, unsigned long vaddr)
{
#if 0
	unsigned int offset = CACHE_COLOUR(vaddr);
	unsigned long to = to_address + (offset << PAGE_SHIFT);

	/*
	 * Discard data in the kernel mapping for the new page
	 * FIXME: needs this MCRR to be supported.
	 */
	__asm__("mcrr	p15, 0, %1, %0, c6	@ 0xec401f06"
	   :
	   : "r" (kaddr),
	     "r" ((unsigned long)kaddr + PAGE_SIZE - L1_CACHE_BYTES)
	   : "cc");

#endif
	/*
	 * Now clear the page using the same cache colour as
	 * the pages ultimate destination.
	 */
	spin_lock(&v6_lock);

#if 0
	set_pte_ext(TOP_PTE(to_address) + offset, pfn_pte(__pa(kaddr) >> PAGE_SHIFT, PAGE_KERNEL), 0);
	flush_tlb_kernel_page(to);
#endif

	cpu_flush_tlb_all();
	clear_page(kaddr);

	spin_unlock(&v6_lock);
}

//DECLARE_CPU_OP(cpu_clear_page, v6_clear_user_page_aliasing);