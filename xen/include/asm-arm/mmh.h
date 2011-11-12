/*
 * pgtable.h
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

#ifndef __ARM_PGTABLE_H__
#define __ARM_PGTABLE_H__

#include <asm/cpu-domain.h>

#define PDE_FLAG_MASK		(0x3FF)

#define PDE_TYPE_FAULT          (0x00)
#define PDE_TYPE_COARSE         (0x01)
#define PDE_TYPE_SECTION        (0x02)
#define PDE_TYPE_FINE           (0x03)
#define PDE_TYPE_MASK		(0x03)

#define PDE_BIT4		(1 << 4)

#define PDE_AP_SRW_UNO          (0x01 << 10)
#define PDE_AP_SRW_URO          (0x02 << 10)
#define PDE_AP_SRW_URW          (0x03 << 10)

#define PDE_BUFFERABLE		(0x04)
#define PDE_CACHEABLE		(0x08)

#define PDE_TEX(x)		((x) <<12)
#define PDE_APX			(1 << 15)
#define PDE_S			(1 << 16)
#define PDE_nG			(1 << 17)

#define PDE_WRITEBACK		(PDE_CACHEABLE | PDE_BUFFERABLE)
#define PDE_WRITETHROUGH	(PDE_CACHEABLE)
#define PDE_SHARED		(0)

#define PDE_DOMAIN_HYPERVISOR   (DOMAIN_HYPERVISOR << 5)
#define PDE_DOMAIN_SUPERVISOR	(DOMAIN_SUPERVISOR << 5)
#define PDE_DOMAIN_USER         (DOMAIN_USER << 5)
#define PDE_DOMAIN_IO           (DOMAIN_IO << 5)

#define PDE_WBWA		(PDE_TEX(1) | PDE_WRITEBACK)

#if 1
#define PDE_TYPE_HYPERVISOR	(PDE_TYPE_SECTION | PDE_DOMAIN_HYPERVISOR | PDE_WBWA | PDE_S | PDE_AP_SRW_UNO | PDE_WRITEBACK)
#else
#define PDE_TYPE_HYPERVISOR	(PDE_TYPE_SECTION | PDE_DOMAIN_HYPERVISOR | PDE_S | PDE_AP_SRW_UNO | PDE_WRITEBACK)
#endif
#define PDE_TYPE_IO		(PDE_TYPE_SECTION | PDE_DOMAIN_IO | PDE_AP_SRW_URW)

/*
 * Definition for Page Table Entries
 */

#define PTE_FLAG_MASK		(0xFFF)

#define PTE_TYPE_FAULT          (0x00)
#define PTE_TYPE_LARGE          (0x01)
#define PTE_TYPE_SMALL          (0x02)
#define PTE_TYPE_TINY           (0x03)
#define PTE_TYPE_EXT		(0x02)

#define PTE_TYPE_MASK		(0x03)

#define PTE_BUFFERABLE          (0x04)
#define PTE_CACHEABLE           (0x08)

#define SECTION_SHIFT		(20)
#define SECTION_SIZE            (1 << SECTION_SHIFT)
#define SECTION_MASK		(~(SECTION_SIZE - 1))

#define PGD_SHIFT               (20)
#define PGT_SHIFT               (12)

#define PGD_SIZE		(PAGE_SIZE * 4)

#define PGD_ALIGN(x)		((x + (0x4000 - 1)) & ~(0x4000 - 1))
#define PGT_ALIGN(x)		((x + (0x1000 - 1)) & ~(0x1000 - 1))

#define PTE_SMALL_AP_MASK	(0xff << 4)
#define PTE_SMALL_AP_UNO_SRO	(0x00 << 4)
#define PTE_SMALL_AP_UNO_SRW	(0x55 << 4)
#define PTE_SMALL_AP_URO_SRW	(0xaa << 4)
#define PTE_SMALL_AP_URW_SRW	(0xff << 4)

#define PTE_EXT_XN		(1 << 0)
#define PTE_EXT_AP_MASK		(3 << 4)
#define PTE_EXT_AP0		(1 << 4)
#define PTE_EXT_AP1		(2 << 4)
#define PTE_EXT_AP_UNO_SRO	(0 << 4)
#define PTE_EXT_AP_UNO_SRW	(PTE_EXT_AP0)
#define PTE_EXT_AP_URO_SRW	(PTE_EXT_AP1)
#define PTE_EXT_AP_URW_SRW	(PTE_EXT_AP1|PTE_EXT_AP0)
#define PTE_EXT_TEX(x)		((x) << 6)
#define PTE_EXT_APX		(1 << 9)
#define PTE_EXT_COHERENT	(1 << 9)
#define PTE_EXT_SHARED		(1 << 10)
#define PTE_EXT_NG		(1 << 11)

#define PGD_ORDER		12
#define PGT_ORDER		8

#define PGD_ENTRIES		(1 << PGD_ORDER)
#define PGT_ENTRIES		(1 << PGT_ORDER)

#define PTE_GUEST_AP_MASK	PTE_EXT_AP_MASK
#define PTE_GUEST_AP_NO		PTE_EXT_AP_UNO_SRW
#define PTE_GUEST_AP_RO		PTE_EXT_AP_URO_SRW
#define PTE_GUEST_AP_RW		PTE_EXT_AP_URW_SRW

#if 0
#define PTE_GUEST_AP_MASK	PTE_SMALL_AP_MASK
#define PTE_GUEST_AP_NO		PTE_SMALL_AP_UNO_SRW
#define PTE_GUEST_AP_RO		PTE_SMALL_AP_URO_SRW
#define PTE_GUEST_AP_RW		PTE_SMALL_AP_URW_SRW
#endif

#define PDE_GUEST_TABLE		(PDE_TYPE_COARSE | PDE_DOMAIN_SUPERVISOR)
#define PDE_VECTOR_TABLE	(PDE_TYPE_COARSE | PDE_DOMAIN_SUPERVISOR)

#ifndef CONFIG_SMP
#define PTE_GUEST_PAGE		(PTE_TYPE_EXT | PTE_EXT_NG | PTE_GUEST_AP_RW | PTE_BUFFERABLE | PTE_CACHEABLE)
#else
#define PTE_GUEST_PAGE		(PTE_TYPE_EXT | PTE_GUEST_AP_RW | PTE_BUFFERABLE | PTE_CACHEABLE)
#endif

#define PTE_VECTOR_PAGE		(PTE_TYPE_EXT | PTE_GUEST_AP_RO | PTE_BUFFERABLE | PTE_CACHEABLE)

#define GRANT_PTE_FLAGS		(PTE_TYPE_EXT | PTE_BUFFERABLE | PTE_CACHEABLE | PTE_GUEST_AP_RW)

#define PTE_FLAG_SHARED_INFO	(PTE_TYPE_EXT | PTE_EXT_TEX(1) | PTE_EXT_XN | PTE_EXT_SHARED | PTE_BUFFERABLE | PTE_CACHEABLE | PTE_GUEST_AP_RW)
//#define PTE_FLAG_SHARED_INFO	(PTE_TYPE_EXT | PTE_EXT_XN | PTE_EXT_SHARED | PTE_BUFFERABLE | PTE_CACHEABLE | PTE_GUEST_AP_RW)

#define TTB_S           (1 << 1)
#define TTB_RGN_WBWA    (1 << 3)

#ifndef CONFIG_SMP
#define TTB_FLAGS       TTB_RGN_WBWA
#else
#define TTB_FLAGS       TTB_RGN_WBWA|TTB_S
#endif


#ifndef __ASSEMBLY__

#include <asm/types.h>

#define MK_PTE(x, flags)	((pte_t) { ((x) & (~PTE_FLAG_MASK)) | flags } )
#define MK_PDE(x, flags)	((pde_t) { ((x) & (~PDE_FLAG_MASK)) | flags } )

#define PGT_IDX(x)		(((x) >> PGT_SHIFT) & (PGT_ENTRIES - 1))
#define PGD_IDX(x)		(((x) >> PGD_SHIFT) & (PGD_ENTRIES - 1))

#define PTE_NONE(pte)		(!pte_val(pte))
#define PTE_PRESENT(pte)	(pte_val(pte) & PTE_TYPE_MASK)
#define PTE_CLEAR(pte)		(pte_val(*(pte)) = 0UL)

#define PDE_NONE(pde)		(!pde_val(pde))
#define PDE_PRESENT(pde)	(pde_val(pde) & PDE_TYPE_MASK)
#define PDE_CLEAR(pde)		(pde_val(*(pde)) = 0UL)

typedef struct { unsigned long pte; } pte_t;
typedef struct { unsigned long pgd; } pde_t;

static inline unsigned long get_ttb(void)
{
	unsigned long temp;
	__asm__ __volatile(
		"mrc	p15, 0, %0, c2, c0, 0" 
		: "=r" (temp) 
		:
		: "memory", "cc"
	);

	return temp;
}

#endif /* __ASSEMBLY__ */

#endif /* __ARM_PGTABLE_H__ */
