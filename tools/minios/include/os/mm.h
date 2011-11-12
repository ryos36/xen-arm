#ifndef _MM_H_
#define _MM_H_

#include <os/os.h>
#include <os/lib.h>

#define L1_FRAME                1
#define L2_FRAME                2

// Modified by hanchan
#define L1_PAGETABLE_SHIFT      12
#define L2_PAGETABLE_SHIFT	20

#define L1_PAGETABLE_ENTRIES	256
#define L2_PAGETABLE_ENTRIES	4096

#define	PADDR_BITS		32
#define	PADDR_MASK		(~0UL)
// end of modified by hanchan

#define L1_MASK  ((1UL << L2_PAGETABLE_SHIFT) - 1)

/* Given a virtual address, get an entry offset into a page table. */
#define l1_table_offset(_a) \
  (((_a) >> L1_PAGETABLE_SHIFT) & (L1_PAGETABLE_ENTRIES - 1))
#define l2_table_offset(_a) \
  (((_a) >> L2_PAGETABLE_SHIFT) & (L2_PAGETABLE_ENTRIES - 1))


#define PAGE_SIZE       (1UL << L1_PAGETABLE_SHIFT)
#define PAGE_SHIFT      L1_PAGETABLE_SHIFT
#define PAGE_MASK       (~(PAGE_SIZE-1))

#define PFN_UP(x)   (((x) + PAGE_SIZE-1) >> L1_PAGETABLE_SHIFT)
#define PFN_DOWN(x) ((x) >> L1_PAGETABLE_SHIFT)
#define PFN_PHYS(x) ((x) << L1_PAGETABLE_SHIFT)

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr)        (((addr)+PAGE_SIZE-1)&PAGE_MASK)

extern unsigned long *phys_to_machine_mapping;
#define pfn_to_mfn(_pfn) (phys_to_machine_mapping[(_pfn)])
static __inline__ unsigned long phys_to_machine(unsigned long phys)
{
    unsigned long machine = pfn_to_mfn(phys >> L1_PAGETABLE_SHIFT);
    machine = (machine << L1_PAGETABLE_SHIFT) | (phys & ~PAGE_MASK);
    return machine;
}

extern unsigned long min_mfn;

#define mfn_to_pfn(_mfn) (machine_to_phys_mapping[(_mfn)-min_mfn])
static __inline__ unsigned long machine_to_phys(unsigned long machine)
{
    unsigned long phys = mfn_to_pfn(machine >> L1_PAGETABLE_SHIFT);
    phys = (phys << L1_PAGETABLE_SHIFT) | (machine & ~PAGE_MASK);
    return phys;
}

/*
#if defined(__x86_64__)
#define VIRT_START              0xFFFFFFFF00000000UL
#elif defined(__i386__)
#define VIRT_START              0xC0000000UL
#endif
*/
//////////////////////////////////////////////////////////////////////
// YOU MUST CHECK THIS VALUE
#define VIRT_START		0x00000000UL
//////////////////////////////////////////////////////////////////////

#define to_phys(x)                 ((unsigned long)(x)-VIRT_START)
#define to_virt(x)                 ((void *)((unsigned long)(x)+VIRT_START))

#define virt_to_pfn(_virt)         (PFN_DOWN(to_phys(_virt)))
#define mach_to_virt(_mach)        (to_virt(machine_to_phys(_mach)))
#define mfn_to_virt(_mfn)          (mach_to_virt(_mfn << PAGE_SHIFT))
#define pfn_to_virt(_pfn)          (to_virt(_pfn << PAGE_SHIFT))

/* Pagetable walking. */
#define pte_to_mfn(_pte)           (((_pte) & (PADDR_MASK&PAGE_MASK)) >> L1_PAGETABLE_SHIFT)
#define pte_to_virt(_pte)          to_virt(mfn_to_pfn(pte_to_mfn(_pte)) << PAGE_SHIFT)

void mm_init(void);
unsigned long alloc_pages(int order);
#define alloc_page()    alloc_pages(0);
void free_pages(void *pointer, int order);

int  update_pagetable(unsigned long virtaddr, unsigned long physaddr, int count);


static __inline__ int get_order(unsigned long size)
{
    int order;
    size = (size-1) >> PAGE_SHIFT;
    for ( order = 0; size; order++ )
        size >>= 1;
    return order;
}

#define _L2_PAGE_SECTION  	0x002U
#define _L2_PAGE_COARSE_PT	0x001U
#define _L2_PAGE_PRESENT	0x003U
#define _L2_PAGE_BUFFERABLE 0x004U
#define _L2_PAGE_CACHEABLE	0x008U

#define _L1_PAGE_SMALL_PG	0x002U
#define _L1_PAGE_PRESENT	0x002U
#define _L1_PAGE_BUFFERABLE 0x004U
#define _L1_PAGE_CACHEABLE	0x008U
#define _L1_PAGE_AP_MANAGER	0xFF0U
#define _L1_PAGE_RW_USER	PTE_SMALL_AP_URW_SRW
#define _L1_PAGE_RO_USER	PTE_SMALL_AP_URO_SRW

/*
 * Domain numbers
 *
 *  DOMAIN_IO     - domain 2 includes all IO only
 *  DOMAIN_USER   - domain 1 includes all user memory only
 *  DOMAIN_KERNEL - domain 0 includes all kernel memory only
 */
#define DOMAIN_KERNEL	0
#define DOMAIN_TABLE	0
#define DOMAIN_USER	1
#define DOMAIN_IO	2

/*
 * Domain types
 */
#define DOMAIN_NOACCESS	0
#define DOMAIN_CLIENT	1
#define DOMAIN_MANAGER	3


/*
 * Hardware page table definitions.
 *
 * + Level 1 descriptor (PMD)
 *   - common
 */
#define PMD_TYPE_MASK		(3 << 0)
#define PMD_TYPE_FAULT		(0 << 0)
#define PMD_TYPE_TABLE		(1 << 0)
#define PMD_TYPE_SECT		(2 << 0)
#define PMD_BIT4		(1 << 4)
#define PMD_DOMAIN(x)		((x) << 5)
#define PMD_PROTECTION		(1 << 9)	/* v5 */
/*
 *   - section
 */
#define PMD_SECT_BUFFERABLE	(1 << 2)
#define PMD_SECT_CACHEABLE	(1 << 3)
#define PMD_SECT_AP_WRITE	(1 << 10)
#define PMD_SECT_AP_READ	(1 << 11)
#define PMD_SECT_TEX(x)		((x) << 12)	/* v5 */
#define PMD_SECT_APX		(1 << 15)	/* v6 */
#define PMD_SECT_S		(1 << 16)	/* v6 */
#define PMD_SECT_nG		(1 << 17)	/* v6 */

#define PMD_SECT_UNCACHED	(0)
#define PMD_SECT_BUFFERED	(PMD_SECT_BUFFERABLE)
#define PMD_SECT_WT		(PMD_SECT_CACHEABLE)
#define PMD_SECT_WB		(PMD_SECT_CACHEABLE | PMD_SECT_BUFFERABLE)
#define PMD_SECT_MINICACHE	(PMD_SECT_TEX(1) | PMD_SECT_CACHEABLE)
#define PMD_SECT_WBWA		(PMD_SECT_TEX(1) | PMD_SECT_CACHEABLE | PMD_SECT_BUFFERABLE)

/*
 *   - coarse table (not used)
 */

/*
 * + Level 2 descriptor (PTE)
 *   - common
 */
#define PTE_TYPE_MASK		(3 << 0)
#define PTE_TYPE_FAULT		(0 << 0)
#define PTE_TYPE_LARGE		(1 << 0)
#define PTE_TYPE_SMALL		(2 << 0)
#define PTE_TYPE_EXT		(3 << 0)	/* v5 */
#define PTE_BUFFERABLE		(1 << 2)
#define PTE_CACHEABLE		(1 << 3)

/*
 *   - extended small page/tiny page
 */
#define PTE_EXT_AP_MASK		(3 << 4)
#define PTE_EXT_AP_UNO_SRO	(0 << 4)
#define PTE_EXT_AP_UNO_SRW	(1 << 4)
#define PTE_EXT_AP_URO_SRW	(2 << 4)
#define PTE_EXT_AP_URW_SRW	(3 << 4)
#define PTE_EXT_TEX(x)		((x) << 6)	/* v5 */

/*
 *   - small page
 */
#define PTE_SMALL_AP_MASK	(0xff << 4)
#define PTE_SMALL_AP_UNO_SRO	(0x00 << 4)
#define PTE_SMALL_AP_UNO_SRW	(0x55 << 4)
#define PTE_SMALL_AP_URO_SRW	(0xaa << 4)
#define PTE_SMALL_AP_URW_SRW	(0xff << 4)


#define __L2_PAGE_HYPERVISOR_SECT (PMD_TYPE_SECT | PMD_SECT_BUFFERABLE | PMD_SECT_CACHEABLE | PMD_BIT4 | PMD_DOMAIN(DOMAIN_KERNEL) | PMD_SECT_AP_WRITE | PMD_SECT_AP_READ)
#define __L2_PAGE_HYPERVISOR_TABLE (PMD_TYPE_TABLE | PMD_BIT4 | PMD_DOMAIN(DOMAIN_KERNEL))
#define __L1_PAGE_HYPERVISOR_SMALL 	(PTE_TYPE_SMALL | PTE_BUFFERABLE | PTE_CACHEABLE | PTE_SMALL_AP_UNO_SRW)

#define __L2_PAGE_USER_SECT (PMD_TYPE_SECT | PMD_SECT_BUFFERABLE | PMD_SECT_CACHEABLE | PMD_BIT4 | PMD_DOMAIN(DOMAIN_USER) | PMD_SECT_AP_WRITE | PMD_SECT_AP_READ)
#define __L2_PAGE_USER_TABLE (PMD_TYPE_TABLE | PMD_BIT4 | PMD_DOMAIN(DOMAIN_USER))
#define __L1_PAGE_USER_SMALL 	(PTE_TYPE_SMALL | PTE_BUFFERABLE | PTE_CACHEABLE | PTE_SMALL_AP_URW_SRW)
#define __L1_PAGE_USER_SMALL_RO	(PTE_TYPE_SMALL | PTE_BUFFERABLE | PTE_CACHEABLE | PTE_SMALL_AP_URO_SRW)



/* 
 * machine-physical conversion macros
 *  - originally from linux-2.6-xen-sparse/include/asm-i386/mach-xen/asm/page.h
 *
 */

#define INVALID_P2M_ENTRY	(~0UL)
#define FOREIGN_FRAME_BIT	(3UL<<30)
#define FOREIGN_FRAME(m)	((m) | FOREIGN_FRAME_BIT)

#endif /* _MM_H_ */
