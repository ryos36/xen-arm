/*
 * page.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *          SungKwan Heo <sk.heo@samsung.com>
 *          JaeMin Ryu  <jm77.ryu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __ARM_PAGE_H__
#define __ARM_PAGE_H__

#include <asm/config.h>
#include <asm/types.h>
#include <asm/memmap.h>
#include <asm/pgtable.h>

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT		12
#define PAGE_SIZE		(1 << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr)	(((addr) + PAGE_SIZE - 1) & PAGE_MASK)

#define PFN_DOWN(x)		( (x)>> PAGE_SHIFT )
#define PFN_UP(x)		( ((x)+PAGE_SIZE-1) >> PAGE_SHIFT )

#define round_pgup(_p)		(((_p)+(PAGE_SIZE-1))&PAGE_MASK)
#define round_pgdown(_p)	((_p)&PAGE_MASK)

#ifndef __ASSEMBLY__
#include <xen/lib.h>

#define clear_page(_p)		memset((void *)(_p), 0, PAGE_SIZE)
#define copy_page(_t, _f)	memcpy((void *)(_t), (void *)(_f), PAGE_SIZE);

static inline int get_order_from_bytes(unsigned long size)
{
	int order;

	size = (size - 1) >> PAGE_SHIFT;
	for ( order = 0; size; order++ )
		size >>= 1;

	return order;
}

static inline int get_order_from_pages(unsigned long nr_pages)
{
	int order;

	nr_pages--;
	for ( order = 0; nr_pages; order++ )
		nr_pages >>= 1;

	return order;
}

/* Convert between Xen-heap virtual addresses and machine addresses. */
#define PAGE_OFFSET		(0xFF000000)
#define va_to_ma(va)		((unsigned long)(va) - PAGE_OFFSET + MEMMAP_HYPERVISOR_BASE)
#define ma_to_va(ma)		((unsigned long)(ma) + PAGE_OFFSET - MEMMAP_HYPERVISOR_BASE)

#define pfn_valid(_pfn)		(((_pfn) >= min_page) && ((_pfn) <= max_page))

#define pfn_to_page(_pfn)	(frame_table + ((_pfn) - min_page))
#define phys_to_page(addr)	(pfn_to_page((addr) >> PAGE_SHIFT))
#define virt_to_page(addr)	(phys_to_page(va_to_ma(addr)))

#define pfn_to_phys(pfn)	((unsigned long)((pfn)) << PAGE_SHIFT)
#define phys_to_pfn(addr)	((unsigned long)((addr) >> PAGE_SHIFT))

#define page_to_pfn(_page)	((unsigned long)((_page + min_page) - frame_table ))
#define page_to_phys(page)	(page_to_pfn(page) << PAGE_SHIFT)
#define page_to_virt(_page)	phys_to_virt(page_to_phys(_page))

#define page_to_mfn(_page)      page_to_pfn(_page)

#define mfn_to_page(_mfn)	pfn_to_page(_mfn)
#define mfn_valid(_mfn)		pfn_valid(_mfn)

#define GET_HVT_PAGE(pgd)		\
({					\
	ma_to_va(pte_val(exception_table_vector[PGT_IDX(VECTORS_BASE)]) & PAGE_MASK);		\
})

/*
 *  virt_to_phys    -   map virtual addresses to physical
 *  @address: address to remap
 *
 *  The returned physical address is the physical (CPU) mapping for
 *  the memory address given. It is only valid to use this function on
 *  addresses directly mapped or allocated via xmalloc.
 *
 *  This function does not give bus mappings for DMA transfers. In
 *  almost all conceivable cases a device driver should not be using
 *  this function
 */
static inline unsigned long virt_to_phys(volatile void * address)
{
	return va_to_ma(address);
}

/*
 *  phys_to_virt    -   map physical address to virtual
 *  @address: address to remap
 *
 *  The returned virtual address is a current CPU mapping for
 *  the memory address given. It is only valid to use this function on
 *  addresses that have a kernel mapping
 *
 *  This function does not handle bus mappings for DMA transfers. In
 *  almost all conceivable cases a device driver should not be using
 *  this function
 */
static inline void * phys_to_virt(unsigned long address)
{
	return (void *)ma_to_va(address);
}

#endif /* !__ASSEMBLY__ */
#endif /* __ARM_PAGE_H__ */
