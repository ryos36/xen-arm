/*
 * mm.c
 *
 * Copyright (C) 2008-2011 Samsung Electronics
 *          Sang-bum Suh <sbuk.suh@samsung.com>
 *          SungKwan Heo <sk.heo@samsung.com>
 *          JaeMin Ryu   <jm77.ryu@samsung.com>
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

#include <xen/lib.h>
#include <xen/types.h>
#include <xen/cpumask.h>
#include <xen/list.h>
#include <xen/kernel.h>
#include <xen/string.h>
#include <xen/errno.h>
#include <xen/sched.h>
#include <xen/mm.h>
#include <xen/domain_page.h>
#include <xen/irq_cpustat.h>
#include <xen/event.h>
#include <xen/iocap.h>
#include <xen/perfc.h>
#include <xen/guest_access.h>
#include <security/acm/acm_hooks.h>
#include <asm/mmu.h>
#include <asm/memory.h>
#include <asm/memmap.h>
#include <asm/cpu-ops.h>

#define VERBOSE 1

#define MMU_UPDATE_PREEMPTED          (~(~0U>>1))

#ifdef VERBOSE
# define MEM_LOG(_f,_a...)                           \
	printf("DOM%u: (file=mm.c, line=%d)" _f "\n", current->domain->domain_id, __LINE__, ## _a)
#else
# define MEM_LOG(_f,_a...)   ((void)0)
#endif

static unsigned long mpt_size;

static struct percpu_info {
#define DOP_FLUSH_TLB			(1 << 0)
#define DOP_FLUSH_CACHE			(1 << 1)
#define DOP_FLUSH_BRANCH_TARGET_CACHE	(1 << 2)

	unsigned long deferred_ops;

	/* If non-NULL, specifies a foreign subject domain for some operations. */
	struct domain *foreign;
	unsigned long dummy[6];
} __cacheline_aligned percpu_info[NR_CPUS] = { 0, };

extern struct domain dom_xen, dom_io;
/* Frame table and its size in pages. */
struct page_info *frame_table;
unsigned long min_page, max_page, total_pages;

void register_memory_bank(unsigned long base, unsigned long size)
{
	struct memory_bank *bank;

	/*
	 * Ensure that start/size are aligned to a page boundary.
	 * Size is appropriately rounded down, start is rounded up.
	 */
	size -= base & ~PAGE_MASK;

	bank = &system_memory.banks[system_memory.nr_banks++];

	bank->base = PAGE_ALIGN(base);
	bank->size  = size & PAGE_MASK;
	bank->node  = 0;
}

#ifdef MEMORY_GUARD
void memguard_init(void)
{
}

void memguard_guard_range(void *p, unsigned long l)
{

}

void memguard_unguard_range(void *p, unsigned long l)
{
}

#endif

void share_xen_page_with_guest(struct page_info *page, struct domain *d, int readonly)
{
	if(page_get_owner(page) == d)
		return;

	set_gpfn_from_mfn(page_to_mfn(page), INVALID_M2P_ENTRY);

	spin_lock(&d->page_alloc_lock);

	/* The incremented type count pins as writable or read-only. */
	page->u.inuse.type_info  = (readonly ? PGT_none : PGT_writable_page);
	page->u.inuse.type_info |= PGT_validated | 1;

	page_set_owner(page, d);
	wmb(); /* install valid domain ptr before updating refcnt. */
	ASSERT(page->count_info == 0);
	page->count_info |= PGC_allocated | 1;

	if ( unlikely(d->xenheap_pages++ == 0) )
		get_knownalive_domain(d);
	list_add_tail(&page->list, &d->xenpage_list);

	spin_unlock(&d->page_alloc_lock);
}

static int pin_page_table(u32 mfn, struct domain *d)
{
	return 1;
}

static int pin_page_directory(u32 mfn, struct domain *d)
{
	return 1;
}

static int unpin_page_table(u32 mfn, struct domain *d)
{
	return 1;
}

void free_page_type(struct page_info *page, unsigned long type)
{
}

void put_page_type(struct page_info *page)
{
}


int get_page_type(struct page_info *page, unsigned long type)
{
    return 1;
}

static inline cpumask_t vcpumask_to_pcpumask(struct domain *d, unsigned long vmask)
{
    unsigned int vcpu_id;
    cpumask_t    pmask = CPU_MASK_NONE;
    struct vcpu *v;

    while ( vmask != 0 )
    {
        vcpu_id = ffs(vmask) - 1;
        vmask &= ~(1UL << vcpu_id);
        if ( (vcpu_id < MAX_VIRT_CPUS) &&
             ((v = d->vcpu[vcpu_id]) != NULL) )
            cpus_or(pmask, pmask, v->vcpu_dirty_cpumask);
    }

    return pmask;
}

static int set_foreigndom(unsigned int cpu, domid_t domid)
{
	struct domain *e, *d;
	int okay = 1;

	ASSERT(percpu_info[cpu].foreign == NULL );

	if(likely(domid == DOMID_SELF))
		return okay;

	switch(domid)
	{
		case DOMID_IO :
			percpu_info[cpu].foreign = &dom_io;
			break;
		case DOMID_XEN:
			percpu_info[cpu].foreign = &dom_xen;
			break;
		default :
			d = current->domain;
			if( !IS_PRIV(d) )
			{
				MEM_LOG("Dom %u cannot set foreign dom", d->domain_id);
				okay = 0;
				break;
			}

			e = find_domain_by_id(domid);
			if(e  == NULL)
			{
				MEM_LOG("Unknown domain '%u'", domid);
				okay = 0;
				break;
			}

			percpu_info[cpu].foreign = e;
			break;
	}

	return okay;
}

int inspect_guest_tables(struct vcpu *v)
{
	return 1;
}

void load_contextid(struct vcpu *v, unsigned long contextid)
{
	v->arch.ctx.contextidr = contextid;
	set_contextidr(contextid);
}

int new_guest_ptbase(unsigned long ptr, int need_flush, int asid)
{
	int rc, i;
	struct vcpu *v = current;
	struct domain *d = v->domain;

	if ( unlikely(!mfn_valid(ptr >> PAGE_SHIFT)) )
	{
		printf("Access to invalid address :  %lx\n", ptr);
		domain_crash(d);

		return 0;
	}

	v->arch.ctx.ttbr0 = ptr;
	load_contextid(v, asid);
	cpu_switch_ttb(v->arch.ctx.ttbr0, need_flush);

	return 1;
}

int do_mmuext_op(XEN_GUEST_HANDLE(mmuext_op_t) uops, 
		 unsigned int count,
		 XEN_GUEST_HANDLE(uint) pdone,
		 unsigned int foreigndom)
{
	struct mmuext_op op;
	int rc = 0, i = 0, okay, cpu = smp_processor_id();
	unsigned int done = 0;
	struct vcpu *v = current;
	struct domain *d = v->domain;

	if ( unlikely(count & MMU_UPDATE_PREEMPTED) )
	{
		count &= ~MMU_UPDATE_PREEMPTED;
		if ( unlikely(!guest_handle_is_null(pdone)) )
			copy_from_guest(&done, pdone, 1);
	}

	if ( !set_foreigndom(cpu, foreigndom) )
	{
		printk("set_foreigndom failed\n");
		rc = -EINVAL;
		goto out;
	}

	if ( unlikely(!guest_handle_okay(uops, count)) )
	{
		printk("guest_handle failed\n");
		rc = -EFAULT;
		goto out;
	}

	for ( i = 0; i < count; i++ )
	{
		if ( unlikely(copy_from_guest(&op, uops, 1) != 0) )
		{
			MEM_LOG("Bad copy_from_guest");
			rc = -EFAULT;
			break;
		}

		okay = 1;

		acm_mmuext_op();

		switch ( op.cmd )
		{
			case MMUEXT_PIN_L1_TABLE:
				okay = pin_page_table(op.arg1.mfn, d);
				break;
			case MMUEXT_PIN_L2_TABLE:
				okay = pin_page_directory(op.arg1.mfn, d);
				break;
			case MMUEXT_UNPIN_TABLE:
				okay = unpin_page_table(op.arg1.mfn, d);
				break;
			case MMUEXT_NEW_BASEPTR:
				new_guest_ptbase(op.arg1.mfn, op.arg2.flags, op.arg3.flags);
				break;
			case MMUEXT_FLUSH_TLB_ALL:
				local_flush_tlb_all(op.arg1.flags);
				break;
			case MMUEXT_FLUSH_TLB_MVA:
				local_flush_tlb_range(op.arg1.mva, op.arg1.mva, op.arg2.flags);
				break;
			case MMUEXT_FLUSH_TLB_ENTRIES:
				local_flush_tlb_range(op.arg1.start, op.arg2.end, op.arg3.flags);
				break;
			case MMUEXT_FLUSH_TLB_ASID:
				//asm("mcr p15, 0, %0, c8, c6, 2" :: "r"(op.arg1.asid) : "cc");
				asm("mcr p15, 0, %0, c8, c3, 2" :: "r"(op.arg1.asid) : "cc");
				asm("mcr p15, 0, %0, c7, c5, 6" :: "r"(0) : "cc");
				dsb();
				break;
			case MMUEXT_FLUSH_CACHE_ALL:
				cpu_flush_cache_all(op.arg1.flags);
				break;
			case MMUEXT_COHERENT_KERN_RANGE:
				cpu_coherent_range(op.arg1.start, op.arg2.end, op.arg3.flags);
				break;
			case MMUEXT_COHERENT_USER_RANGE:
				cpu_coherent_range(op.arg1.start, op.arg2.end, op.arg3.flags);
				break;
			case MMUEXT_CLEAN_CACHE_RANGE:
				cpu_clean_cache_range(op.arg1.start, op.arg2.end, op.arg3.flags);
				break;
			case MMUEXT_FLUSH_CACHE_RANGE:
				cpu_flush_cache_range(op.arg1.start, op.arg2.end, op.arg3.flags);
				break;
			case MMUEXT_DMAC_INV_RANGE:
				cpu_invalidate_dma_range(op.arg1.start, op.arg2.end, op.arg3.flags);
				break;
			case MMUEXT_DMAC_CLEAN_RANGE:
				cpu_clean_dma_range(op.arg1.start, op.arg2.end, op.arg3.flags);
				break;
			case MMUEXT_DMAC_FLUSH_RANGE:
				cpu_flush_dma_range(op.arg1.start, op.arg2.end, op.arg3.flags);
				break;
			case MMUEXT_CLEAR_USER_PAGE:
				while((op.arg1.linear_addr & 0x7) != 0);
				cpu_clear_page((void *)op.arg1.mva, op.arg2.mva);
				break;
			case MMUEXT_COPY_USER_PAGE:
				while((op.arg1.linear_addr & 0x7) != 0);
				cpu_copy_page((void *)op.arg1.mva, (void *)op.arg2.mva, op.arg3.flags);
				break;
			case MMUEXT_GET_PGD:
				op.arg1.phys_addr = get_ttbr();
				if ( copy_to_guest(uops, &op, 1) )
					return -EFAULT;
				break;
			case MMUEXT_LOAD_ASID:
				load_contextid(v, op.arg1.asid);
				break;
			default:
				MEM_LOG("Invalid extended pt command %d", op.cmd);
				okay = 0;
				break;
		}

		if ( unlikely(!okay) )
		{
			rc = -EINVAL;
			break;
		}

		guest_handle_add_offset(uops, 1);
	}


out:
	/* Add incremental work we have done to the @done output parameter. */
	if ( unlikely(!guest_handle_is_null(pdone)) )
	{
		done += i;
		copy_to_guest(pdone, &done, 1);
	}

	return rc;
}

int do_mmu_update(XEN_GUEST_HANDLE(mmu_update_t) ureqs,
		  unsigned int count, 
		  XEN_GUEST_HANDLE(uint) pdone,
		  unsigned int foreigndom)
{
	struct mmu_update req;
	int rc = 0, okay = 1, i = 0;
	unsigned int done = 0;

	struct domain *d = current->domain;

	for ( i = 0; i < count; i++ )
	{
		if ( unlikely(copy_from_guest(&req, ureqs, 1) != 0) )
		{
			MEM_LOG("Bad copy_from_guest");
			rc = -EFAULT;
			break;
		}

		*(unsigned long *)req.flags = req.val;
		pte_sync(req.flags);

		okay = 1;

		if ( unlikely(!okay) )
		{
			rc = -EINVAL;
			break;
		}

		guest_handle_add_offset(ureqs, 1);
	}

out:

	/* Add incremental work we have done to the @done output parameter. */
	if ( unlikely(!guest_handle_is_null(pdone)) )
	{
		done += i;
		copy_to_guest(pdone, &done, 1);
	}

	return rc;
}

int do_update_va_mapping(u32 va, u32 flags, u64 val64)
{
#if 0
    pte_t val = l1e_from_intpte(val64);
    struct vcpu   *v   = current;
    struct domain *d   = v->domain;
    unsigned int   cpu = smp_processor_id();
    unsigned long  vmask, bmap_ptr;
    cpumask_t      pmask;
    int            rc  = 0;

    perfc_incrc(calls_to_update_va);
    
	while(1);

	if ( unlikely(!__addr_ok(va)) )
		return -EINVAL;
         
	LOCK_BIGLOCK(d);

	while(1);

	if ( unlikely(!modify_pte(get_pl1pte_from_virtaddr(v, va), val)) )
		rc = -EINVAL;

    switch ( flags & UVMF_FLUSHTYPE_MASK )
    {
    case UVMF_TLB_FLUSH:
        switch ( (bmap_ptr = flags & ~UVMF_FLUSHTYPE_MASK) )
        {
        case UVMF_LOCAL:
            local_flush_tlb();
            break;
        case UVMF_ALL:
            flush_tlb_mask(d->domain_dirty_cpumask);
            break;
        default:
            if ( unlikely(get_user(vmask, (unsigned long *)bmap_ptr)) )
                rc = -EFAULT;
            pmask = vcpumask_to_pcpumask(d, vmask);
            //flush_tlb_mask(pmask);
            break;
        }
        break;

    case UVMF_INVLPG:
        switch ( (bmap_ptr = flags & ~UVMF_FLUSHTYPE_MASK) )
        {
        case UVMF_LOCAL:
            local_flush_tlb_one(va);
            break;
        case UVMF_ALL:
            flush_tlb_one_mask(d->domain_dirty_cpumask, va);
            break;
        default:
            if ( unlikely(get_user(vmask, (unsigned long *)bmap_ptr)) )
                rc = -EFAULT;
            pmask = vcpumask_to_pcpumask(d, vmask);
            //flush_tlb_one_mask(pmask, va);
            break;
        }
        break;
    }

    UNLOCK_BIGLOCK(d);

    return rc;
#endif
	return 1;
}

static int create_grant_pte_mapping(
    unsigned long pte_addr, pte_t new, struct vcpu *v)
{
    int rc = GNTST_okay;
    void *va;
    unsigned long gmfn, mfn;
    struct page_info *page;
    u32 type_info;
    pte_t old;
    struct domain *d = v->domain;

    gmfn = pte_addr >> PAGE_SHIFT;
    mfn = gmfn;

    if ( unlikely(!mfn_valid(mfn)) )
    {
        MEM_LOG("Invalid MFN 0x%x", mfn);
        return GNTST_general_error;
    }

    page = mfn_to_page(mfn);

    if ( unlikely(!get_page(page, current->domain)) )
    {
        MEM_LOG("Could not get page for normal update");
        return GNTST_general_error;
    }
    
    va = map_domain_page(mfn);
    va = (void *)((unsigned long)va + (pte_addr & ~PAGE_MASK));
    page = pfn_to_page(mfn);

    type_info = page->u.inuse.type_info;
    if ( ((type_info & PGT_type_mask) != PGT_page_table) ||
         !get_page_type(page, type_info & PGT_type_mask) )
    {
        MEM_LOG("Grant map attempted to update a non-L1 page");
        rc = GNTST_general_error;
        goto failed;
    }

    consistent_write(va, pte_val(new));

#if 0
    put_page_from_pte(old, d);
#endif
    put_page_type(page);
 
 failed:
    unmap_domain_page(va);
    put_page(page);
    return rc;
}

static int destroy_grant_pte_mapping(
    unsigned long addr, unsigned long frame, struct domain *d)
{
    int rc = GNTST_okay;
    void *va;
    unsigned long gmfn, mfn;
    struct page_info *page;
    u32 type_info;
    pte_t old;

    gmfn = addr >> PAGE_SHIFT;
    mfn = gmfn;

    if ( unlikely(!mfn_valid(mfn)) )
    {
        MEM_LOG("Invalid MFN 0x%x", mfn);
        return GNTST_general_error;
    }

    page = mfn_to_page(mfn);

    if ( unlikely(!get_page(page, current->domain)) )
    {
        MEM_LOG("Could not get page for normal update");
        return GNTST_general_error;
    }
    
    va = map_domain_page(mfn);
    va = (void *)((unsigned long)va + (addr & ~PAGE_MASK));
    page = pfn_to_page(mfn);

    type_info = page->u.inuse.type_info;
    if ( ((type_info & PGT_type_mask) != PGT_page_table) ||
         !get_page_type(page, type_info & PGT_type_mask) )
    {
        MEM_LOG("Grant map attempted to update a non-L1 page");
        rc = GNTST_general_error;
        goto failed;
    }

    if ( __copy_from_user(&old, (pte_t *)va, sizeof(old)) )
    {
        put_page_type(page);
        rc = GNTST_general_error;
        goto failed;
    }
    
    /* Check that the virtual address supplied is actually mapped to frame. */
    if ( unlikely((pte_val(old) >> PAGE_SHIFT) != frame) )
    {
        MEM_LOG("PTE entry %lx for address %lx doesn't match frame %lx",
                (unsigned long)pte_val(old), addr, frame);
        put_page_type(page);
        rc = GNTST_general_error;
        goto failed;
    }

    consistent_write(va, 0);

#if 0
    /* Delete pagetable entry. */
    if ( unlikely(__put_user(0, (intpte_t *)va)))
    {
        MEM_LOG("Cannot delete PTE entry at %p", va);
        put_page_type(page);
        rc = GNTST_general_error;
        goto failed;
    }
#endif

    put_page_type(page);

 failed:
    unmap_domain_page(va);
    put_page(page);
    return rc;
}

static int update_va_mapping(struct vcpu *v, unsigned long va, pte_t new)
{
	pde_t *pgd;
	pte_t *pte;
	unsigned long mfn;

	cpu_flush_cache_all(0);

	mfn = (v->arch.ctx.ttbr0 + (PGD_IDX(va) << 2)) >> PAGE_SHIFT;
	pgd = (pde_t *)map_domain_page(mfn);

	mfn = pde_val(pgd[PGD_IDX(va) & ((~PAGE_MASK) >> 2)]) & 0xFFFFFC00;

	pte = (pte_t *)map_domain_page(mfn >> PAGE_SHIFT);
	pte += ((mfn & (~PAGE_MASK)) >> 2);

	pte = &pte[PGT_IDX(va) & ((~PAGE_MASK) >> 2)];

#if 0
	if( pte_val(*pte) != 0 )
	{
		printf("PTE has entry.\n");
		return -1;
	}
#endif

	*pte = new;

	unmap_domain_page(pgd);
	unmap_domain_page(pte);

	cpu_flush_cache_all(0);
	local_flush_tlb_all(TLBF_ITLB | TLBF_DTLB);

	return 0;
}

static int create_grant_va_mapping(vaddr_t va, pte_t new, struct vcpu *v)
{
#if 0
	pte_t *ptr, old;
	struct domain *d = v->domain;

	printf("VA = 0x%x\n", va);

	ptr = get_pl1pte_from_virtaddr(v, va);

	if ( unlikely(__copy_from_user(&old, ptr, sizeof(old)) != 0))
		return GNTST_general_error;

	consistent_write(ptr, pte_val(new));

#if 0
put_page_from_pte(old, d);
#endif

	return GNTST_okay;
#endif

    return update_va_mapping(v, va, new);
}

static int destroy_grant_va_mapping(unsigned long addr, unsigned long frame, struct vcpu *v)
{
#if 0
    pte_t *ptr, old;
    
    ptr = get_pl1pte_from_virtaddr(v, addr);
    
    if ( unlikely(__get_user(old.pte, &ptr->pte) != 0) )
    {
        MEM_LOG("Could not find PTE entry for address %lx", addr);
        return GNTST_general_error;
    }

    /*
     * Check that the virtual address supplied is actually mapped to
     * frame.
     */
    if ( unlikely((pte_val(old) >> PAGE_SHIFT) != frame) )
    {
        MEM_LOG("PTE entry %lx for address %lx doesn't match frame %lx",
                (pte_val(old) >> PAGE_SHIFT), addr, frame);
        return GNTST_general_error;
    }

    /* Delete pagetable entry. */
    if ( unlikely(__put_user(0, &ptr->pte)) )
    {
        MEM_LOG("Cannot delete PTE entry at %p", (unsigned long *)ptr);
        return GNTST_general_error;
    }
    
    return 0;
#endif

	return update_va_mapping(v, addr, MK_PTE(0, 0));
}

int create_grant_host_mapping(unsigned long addr, unsigned long frame, unsigned int flags)
{
    pte_t pte;
    unsigned long pte_flags = PTE_GRANT_PAGE;

        
    if ( (flags & GNTMAP_application_map) )
        pte_flags |= PTE_GUEST_AP_RW;
    if ( !(flags & GNTMAP_readonly) )
        pte_flags |= PTE_GUEST_AP_RO;

    pte = MK_PTE(frame << PAGE_SHIFT, pte_flags);

    if ( flags & GNTMAP_contains_pte ) {
		printf("GNTMAP_contains_pte\n");
        return create_grant_pte_mapping(addr, pte, current);
	}

    return create_grant_va_mapping(addr, pte, current);
}

int destroy_grant_host_mapping(unsigned long addr, unsigned long frame, unsigned int flags)
{

    if ( flags & GNTMAP_contains_pte ) {
        return destroy_grant_pte_mapping(addr, frame, current->domain);
    }

    return destroy_grant_va_mapping(addr, frame, current);
}

int steal_page_for_grant_transfer(struct domain *d, struct page_info *page)
{
    u32 _d, _nd, x, y;

    spin_lock(&d->page_alloc_lock);

    /*
     * The tricky bit: atomically release ownership while there is just one 
     * benign reference to the page (PGC_allocated). If that reference 
     * disappears then the deallocation routine will safely spin.
     */
    _d  = pickle_domptr(d);
    _nd = page->u.inuse._domain;
    y   = page->count_info;
    do {
        x = y;
        if (unlikely((x & (PGC_count_mask|PGC_allocated)) !=
                     (1 | PGC_allocated)) || unlikely(_nd != _d)) { 
            DPRINTK("gnttab_transfer: Bad page %p: ed=%p(%u), sd=%p, caf=0x%x, taf=0x%x\n", 
                    (void *) page_to_pfn(page),
                    d, d->domain_id, unpickle_domptr(_nd), x, 
                    page->u.inuse.type_info);
            spin_unlock(&d->page_alloc_lock);
            return -1;
        }
      
		local_irq_disable();

		if( (x == page->count_info) && (_d == page->u.inuse._domain) )  {
			page->u.inuse._domain = _d;
			y  = x;
			_nd =_d ;
		}
		else {
			y  = page->count_info;
			_nd = page->u.inuse._domain;
		}

		local_irq_enable();


    } while (unlikely(_nd != _d) || unlikely(y != x));

    /*
     * Unlink from 'd'. At least one reference remains (now anonymous), so 
     * noone else is spinning to try to delete this page from 'd'.
     */
    d->tot_pages--;
    list_del(&page->list);

    spin_unlock(&d->page_alloc_lock);

    return 0;
}

long arch_memory_op(int op, XEN_GUEST_HANDLE(void) arg)
{
	return 0;
}
