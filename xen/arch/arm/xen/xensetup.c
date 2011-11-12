/*
 * xensetup.c
 *
 * Copyright (C) 2008-2011 Samsung Electronics
 *          Sang-bum Suh <sbuk.suh@samsung.com>
 *          JooYoung Hwang <jooyoung.hwang@samsung.com>
 *     	    Jaemin Ryu <jm77.ryu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as
 * published by the Free Software Foundation.
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

#include <xen/config.h>
#include <xen/init.h>
#include <xen/sched.h>
#include <xen/mm.h>
#include <xen/compile.h>
#include <xen/string.h>
#include <xen/lib.h>
#include <xen/preempt.h>
#include <public/version.h>
#include <public/sched.h>
#include <security/acm/policy_conductor.h>
#include <security/acm/acm_hooks.h>
#include <security/ssm-xen/sra_func.h>
#include <asm/core.h>

#include <asm/mmu.h>
#include <asm/memmap.h>
#include <asm/trap.h>
#include <asm/memory.h>
#include <asm/uaccess.h>
#include <asm/cpu-ops.h>
#include <asm/platform.h>

#ifdef CONFIG_GCOV_XEN
#include <xen/gcov.h>
#endif

#define DOM_CREATE_SUCCESS	1
#define DOM_CREATE_FAIL		0

#define BANNER "\n\rXen/ARM virtual machine monitor for %s\n\r"		\
	       "Copyright (C) 2007 Samsung Electronics Co, Ltd. All Rights Reserved.\n" \

struct domain *idle_domain;
struct domain dom_xen = {
	.refcnt = 1,
	.domain_id = DOMID_XEN,
	.big_lock = SPIN_LOCK_UNLOCKED,
	.page_alloc_lock = SPIN_LOCK_UNLOCKED,
};

struct domain dom_io = {
	.refcnt = 1,
	.domain_id = DOMID_IO,
	.big_lock = SPIN_LOCK_UNLOCKED,
	.page_alloc_lock = SPIN_LOCK_UNLOCKED,
};

struct meminfo system_memory = {0,};

unsigned long xenheap_phys_start;
unsigned long xenheap_phys_end;

struct domain_partition {
        unsigned long   guest_memory_start;
        unsigned long   guest_memory_size;
        unsigned long   elf_image_address;
        unsigned long   elf_image_size;
        unsigned long   initrd_address;
        unsigned long   initrd_size;
        unsigned long   command_line_address;
        unsigned long   stack_start;
};

struct domain_partition domain_partitions[4]= {
{
	MEMMAP_GUEST0_PARTITION_BASE,
	MEMMAP_GUEST0_PARTITION_SIZE,
	MEMMAP_GUEST0_IMAGE_BASE,
	MEMMAP_GUEST0_IMAGE_SIZE,
	0,
	0,
	0,
	0 
},
{
	MEMMAP_GUEST1_PARTITION_BASE, 
	MEMMAP_GUEST1_PARTITION_SIZE,
	MEMMAP_GUEST1_IMAGE_BASE, 
	MEMMAP_GUEST1_IMAGE_SIZE,
	0,
	0,
	0,
	0 
},
{
	MEMMAP_GUEST2_PARTITION_BASE,
	MEMMAP_GUEST2_PARTITION_SIZE,
	MEMMAP_GUEST2_IMAGE_BASE, 
	MEMMAP_GUEST2_IMAGE_SIZE,
	0,
	0,
	0,
	0
},
{
	MEMMAP_GUEST3_PARTITION_BASE,
	MEMMAP_GUEST3_PARTITION_SIZE,
	MEMMAP_GUEST3_IMAGE_BASE, 
	MEMMAP_GUEST3_IMAGE_SIZE,
	0,
	0,
	0,
	0
},
};

pte_t __attribute__((__section__(".bss.page_aligned"))) page_table[1024];

pte_t *exception_table_vector = &page_table[0];
pte_t *shared_info_vector = &page_table[256];
pte_t *mapcache_table_vector = &page_table[512];

static void prepare_console(void)
{
        init_console();

}

static void prepare_subsystems(void)
{
	initcall_t *call;

	for ( call = &__initcall_start; call < &__initcall_end; call++ )
	{
		(*call)();
	}
}

void arch_get_xen_caps(xen_capabilities_info_t info)
{
	char *p = info;

	p += snprintf(p, sizeof(32), "xen-%d.%d-arm ", XEN_VERSION, XEN_SUBVERSION);

	*(p-1) = 0;

	BUG_ON((p - info) > sizeof(xen_capabilities_info_t));

}

static unsigned long find_lowest_pfn(struct meminfo *mi)
{
	int i;
	unsigned long start = 0xFFFFFFFF;

	for(i = 0; i < mi->nr_banks; i++) {
		struct memory_bank *bank = &mi->banks[i];

		if(bank->base < start) {
			start = bank->base;
		}
	}

	return start >> PAGE_SHIFT;
}

static unsigned long find_highest_pfn(struct meminfo *mi)
{
	int i;
	unsigned long end = 0;

	for(i = 0; i < mi->nr_banks; i++) {
		struct memory_bank *bank = &mi->banks[i];

		if(end < bank->base + bank->size) {
			end = bank->base + bank->size;
		}
	}

	return end >> PAGE_SHIFT;
}

static void prepare_allocator()
{
	unsigned long nr_pages = 0;
	unsigned long i, s, e;
	unsigned long xen_pstart;
	unsigned long xen_pend;

	/*
	 * Memory holes will be reserved during
	 * init_boot_pages().
	 */
	min_page = find_lowest_pfn(&system_memory);
	max_page = find_highest_pfn(&system_memory);

	xen_pstart = min_page << PAGE_SHIFT; 
	xen_pend = max_page << PAGE_SHIFT;

	/*
	 * | Xen Heap         |
	 * +------------------+ _end
	 * | frame table      |
	 * +------------------+
	 * | bss section      |
	 * +------------------+
	 * | data section     |
	 * +------------------+
	 * | code section     |
	 * +------------------+ 0xFF008000
	 * | root trans table |
	 */
	/* Initialise boot-time allocator with all RAM situated after modules. */
	frame_table = (struct page_info *)(round_pgup(((unsigned long)(&_end))));
	nr_pages = PFN_UP((max_page - min_page) * sizeof(struct page_info));

	memset(frame_table, 0, nr_pages << PAGE_SHIFT);

	xenheap_phys_start = init_boot_allocator(va_to_ma(frame_table) + (nr_pages << PAGE_SHIFT));
	xenheap_phys_end   = xenheap_phys_start + MEMMAP_HYPERVISOR_SIZE;
	
	/* Initialise the DOM heap, skipping RAM holes. */
	nr_pages = 0;
	for ( i = 0; i < system_memory.nr_banks; i++ ) {
		nr_pages += system_memory.banks[i].size >> PAGE_SHIFT;

		/* Initialise boot heap, skipping Xen heap and dom0 modules. */
		s = system_memory.banks[i].base;
		e = s + system_memory.banks[i].size;
		
#if 0
		if ( s < xenheap_phys_end )
			s = xenheap_phys_end;
		if( e > xen_pend )
			e = xen_pend;
#else
		if ( s < (min_page << PAGE_SHIFT)) 
			s = (min_page << PAGE_SHIFT);
		if ( e > MEMMAP_HYPERVISOR_BASE)
			e = MEMMAP_HYPERVISOR_BASE;
#endif
		init_boot_pages(s, e);
	}

	total_pages = nr_pages;

	end_boot_allocator();

	/* Initialise the Xen heap, skipping RAM holes. */
	nr_pages = 0;
	for ( i = 0; i < system_memory.nr_banks; i++ ) {
		s = system_memory.banks[i].base;
		e = s + system_memory.banks[i].size;
		if ( s < xenheap_phys_start )
			s = xenheap_phys_start;
		if ( e > xenheap_phys_end )
			e = xenheap_phys_end;
		if ( s < e ) {
			nr_pages += (e - s) >> PAGE_SHIFT;
			init_xenheap_pages(s, e);
		}
	}

}

static void prepare_page_tables(void)
{
	int idx;
	pde_t *pgd;

	/* Get translation table base */
	pgd = (pde_t *)(ma_to_va(get_ttbr() & TTB_MASK));

	/* Link page table for exception vectors table */
	idx = PGD_IDX(VECTORS_BASE);
	pgd[idx] = MK_PDE(va_to_ma(&page_table[0]), PDE_VECTOR_TABLE);
	pte_sync(&pgd[idx]);

	/* Link page table for shared_info array */
	idx = PGD_IDX(SHARED_INFO_BASE);
	pgd[idx] = MK_PDE(va_to_ma(&page_table[256]), PDE_GUEST_TABLE);
	pte_sync(&pgd[idx]);

	/* Link mapcache table for shared_info array */
	idx = PGD_IDX(MAPCACHE_VIRT_START);
	pgd[idx] = MK_PDE(va_to_ma(&page_table[512]), PDE_GUEST_TABLE);
	pte_sync(&pgd[idx]);
}

/*
 * All platform-speficif initialization should be completed before start_xen()
 *
 */
asmlinkage void start_xen(struct platform *platform)
{
	unsigned int boot_cpu;

	boot_cpu = smp_processor_id();

	prepare_boot_cpu(boot_cpu);

	prepare_page_tables();

	trap_init();

	prepare_console();

	printk(BANNER, platform->name);

	prepare_allocator();

	sort_extables();


#if CONFIG_GCOV_XEN
	gcov_core_init();
#endif

#ifdef CONFIG_VMM_SECURITY
	if ( sra_init() != 0 )
	  PANIC("Error Secure Repository Agent initialization\n");
#endif

	timer_init();

	init_acm();

	prepare_subsystems();


#ifdef UNIT_TEST
	/* Unit Test Example */
	embunit_test_example();
#endif

	scheduler_init();

	idle_domain = domain_create(IDLE_DOMAIN_ID, 0);

	BUG_ON(idle_domain == NULL);

	set_current(idle_domain->vcpu[0]);

	idle_domain->vcpu[0]->arch.ctx.ttbr0 = (get_ttbr() & TTB_MASK);

	local_irq_enable();
	local_fiq_enable();

#ifdef CONFIG_SMP
	smp_prepare_cpus(NR_CPUS);
#endif
	dom0 = domain_create(0, 0);

	BUG_ON(dom0 == NULL);

	if ( construct_domain(dom0,
			domain_partitions[0].guest_memory_start,
			domain_partitions[0].guest_memory_size,
			domain_partitions[0].elf_image_address,
			domain_partitions[0].elf_image_size,
			domain_partitions[0].initrd_address,
			domain_partitions[0].initrd_size,
			NULL) != 0)
	{
		PANIC("Could not set up DOM0 guest OS\n");
	}

	domain_unpause_by_systemcontroller(dom0);

	start_idle_loop();
}

int get_guest_domain_address( dom0_op_t * dom0_op)
{
	unsigned int domain_id;
	unsigned int ret=0;
	dom0_op_t * op = dom0_op;
	
	domain_id = op->u.guest_image_info.domain;

	/* return guest domain loading physical address */
	op->u.guest_image_info.guest_image_address =
		domain_partitions[domain_id].elf_image_address;

	op->u.guest_image_info.guest_image_size    = 
		domain_partitions[domain_id].elf_image_size;

	return ret;
}

int create_guest_domain( dom0_op_t * dom0_op )
{
	unsigned int domain_id;
	unsigned long guest_va;
	struct domain *dom;

	domain_id = dom0_op->u.guest_image_info.domain;

	guest_va = dom0_op->u.guest_image_info.guest_image_address;

	dom = find_domain_by_id(domain_id);
	if ( dom == NULL )
	{
		PANIC("Could not find the domain structure for DOM guest OS\n");
		return DOM_CREATE_FAIL;	
	}

	dom->store_port   = dom0_op->u.guest_image_info.store_port;
	dom->console_port = dom0_op->u.guest_image_info.console_port;
		
	if ( construct_domain( dom,
			domain_partitions[domain_id].guest_memory_start,
			domain_partitions[domain_id].guest_memory_size,
			domain_partitions[domain_id].elf_image_address,
			domain_partitions[domain_id].elf_image_size,
			domain_partitions[domain_id].initrd_address,
			domain_partitions[domain_id].initrd_size,
			NULL) != 0)           // stack start
	{
		put_domain(dom);
		PANIC("Could not set up DOM1 guest OS\n");
		return DOM_CREATE_FAIL;	
	}

	dom0_op->u.guest_image_info.store_mfn = dom->store_mfn;
	dom0_op->u.guest_image_info.console_mfn = dom->console_mfn;

	printk("store mfn = 0x%x\n", dom->store_mfn);
	printk("console mfn = 0x%x\n", dom->console_mfn);

	put_domain(dom);

	return DOM_CREATE_SUCCESS;	
}
