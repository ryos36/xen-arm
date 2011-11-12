/*
 * mm.h
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

#ifndef __ARM_MM_H__
#define __ARM_MM_H__

#include <xen/cpumask.h>
#include <xen/list.h>
#include <xen/sched.h>
#include <xen/mm.h>
#include <asm/system.h>
#include <asm/io.h>
#include <asm/types.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/tlb.h>
#include <asm/config.h>
#include <asm/atomic.h>
#include <asm/page.h>
#include <asm/cpu-ops.h>
#include <asm/cpu-domain.h>
#include <asm/domain.h>
#include <public/xen.h>

#define INVALID_MFN             (~0UL)
#define INVALID_M2P_ENTRY	(~0UL)

#define PFN_ORDER(_pfn)		((_pfn)->u.free.order)

#define PAGE_TYPE(page)		(((page)->u.inuse.type_info) & PGT_type_mask )

#define pickle_domptr(_d)	((u32)(unsigned long)(_d))
#define unpickle_domptr(_d)	((struct domain *)(unsigned long)(_d))
#define PRtype_info		"08lx"

#define page_get_owner(_p)	(unpickle_domptr((_p)->u.inuse._domain))
#define page_set_owner(_p,_d)	((_p)->u.inuse._domain = pickle_domptr(_d))

#define XENSHARE_writable 	0
#define XENSHARE_readonly 	1

 /* The following page types are MUTUALLY EXCLUSIVE. */
#define PGT_none            	(0U<<30) /* no special uses of this page */
#define PGT_page_table		(1U<<30) /* using this page as an leaf table */
#define PGT_page_directory	(2U<<30) /* using this page as an root table */
#define PGT_writable_page	(3U<<30) /* has writable mappings of this page? */
#define PGT_type_mask		(3U<<30) /* Bits 29-31. */

 /* Has this page been validated for use as its current type? */
#define _PGT_validated		29
#define PGT_validated		(1U<<_PGT_validated)
 /* Owning guest has pinned this page to its current type? */
#define _PGT_pinned		28
#define PGT_pinned		(1U<<_PGT_pinned)

 /* 16-bit count of uses of this frame as its current type. */
#define PGT_count_mask		((1U<<16)-1)

 /* Cleared when the owning guest 'frees' this page. */
#define _PGC_allocated		31
#define PGC_allocated		(1U<<_PGC_allocated)

 /* 29-bit count of references to this frame. */
#define PGC_count_mask		((1U<<29)-1)


#define set_gpfn_from_mfn(mfn, pfn) do { } while(0)
#define get_gpfn_from_mfn(mfn)	((mfn))

#define mfn_to_gmfn(_d, mfn)	(mfn)

#define gmfn_to_mfn(_d, gpfn)	(gpfn)

#define write_ptbase(v)		cpu_switch_ttb((v)->arch.ctx.ttbr0, 1);

#define  IS_XEN_HEAP_FRAME(_pfn)		\
({						\
	unsigned long phys;			\
	phys = page_to_phys(_pfn);		\
	((phys >= xenheap_phys_start) && 	\
	 (phys < xenheap_phys_end));		\
})

/*
 * Per-page-frame information.
 * 
 * Every architecture must ensure the following:
 *  1. 'struct page_info' contains a 'struct list_head list'.
 *  2. Provide a PFN_ORDER() macro for accessing the order of a free page.
 */
struct page_info
{
    /* Reference count and various PGC_xxx flags and fields. */
    unsigned long count_info;

    /* Context-dependent fields follow... */
    union {

        /* Page is in use: ((count_info & PGC_count_mask) != 0). */
        struct {
            /* Type reference count and various PGT_xxx flags and fields. */
            unsigned long type_info;
            /* Owner of this page (NULL if page is anonymous). */
            u32 _domain; /* pickled format */
        }inuse;

        /* Page is on a free list: ((count_info & PGC_count_mask) == 0). */
        struct {
            /* Order-size of the free chunk this page is the head of. */
            u32 order;
            /* Mask of possibly-tainted TLBs. */
//            cpumask_t cpumask;
        }free;

    } u;

    /* Each frame can be threaded onto a doubly-linked list. */
    struct list_head list;

};

#ifdef MEMORY_GUARD
void memguard_init(void);
void memguard_guard_range(void *p, unsigned long l);
void memguard_unguard_range(void *p, unsigned long l);
#else
#define memguard_init()                ((void)0)
#define memguard_guard_range(_p,_l)    ((void)0)
#define memguard_unguard_range(_p,_l)  ((void)0)
#endif /* MEMORY_GUARD */

void memguard_guard_stack(void *p);

extern struct page_info *frame_table;
extern unsigned long min_page, max_page;
extern unsigned long total_pages;

void init_frametable(void);

void share_xen_page_with_guest(struct page_info *page, struct domain *d, int readonly);

int alloc_page_type(struct page_info *page, unsigned long type);
void free_page_type(struct page_info *page, unsigned long type);

void put_page_type(struct page_info *page);
int  get_page_type(struct page_info *page, unsigned long type);

void zap_low_mappings(pde_t *base);
void arch_init_memory(void);
void paging_init(void);

static inline void put_page(struct page_info *page)
{
	u32 nx, x, y = page->count_info;

	do {
		x  = y;
		nx = x - 1;
	} while ( unlikely((y = cmpxchg(&page->count_info, x, nx)) != x) );

	if ( unlikely((nx & PGC_count_mask) == 0) ) {
		free_domheap_page(page);
	}

}

static inline int get_page(struct page_info *page,
                           struct domain *domain)
{
	u32 x, nx, y = page->count_info;
	u32 d, nd = page->u.inuse._domain;
	u32 _domain = pickle_domptr(domain);

	do {
		x  = y;
		nx = x + 1;
		d  = nd;
		if ( unlikely((x & PGC_count_mask) == 0) ||  /* Not allocated? */
		     unlikely((nx & PGC_count_mask) == 0) || /* Count overflow? */
		     unlikely(d != _domain) )                /* Wrong owner? */
		{
			printf("shadow mode is not implemented on xen-arm = 0x%x, %x, %x\n", page_to_mfn(page), x, nx);
			while(1);
			return 0;
		}
 	} while ( unlikely((y = cmpxchg((u32 *) &page->count_info, (u32) x, (u32) nx)) != x) );

    return 1;
}

static inline void put_page_and_type(struct page_info *page)
{
    put_page_type(page);
    put_page(page);
}

static inline int get_page_and_type(struct page_info *page,
                                    struct domain *domain,
                                    unsigned long type)
{
    int rc = get_page(page, domain);

    if ( likely(rc) && unlikely(!get_page_type(page, type)) )
    {
        put_page(page);
        rc = 0;
    }

    return rc;
}

#endif /* __ARM_MM_H__ */
