/*
 * domain_page.c
 *
 * Copyright (C) 2008-2011 Samsung Electronics
 *          Sang-bum Suh <sbuk.suh@samsung.com>
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

#include <xen/init.h>
#include <xen/lib.h>
#include <xen/sched.h> 
#include <xen/perfc.h>
#include <xen/domain_page.h>
#include <public/arch-arm.h>
#include <asm/current.h>
#include <asm/config.h>
#include <asm/domain.h>
#include <asm/hardirq.h>
#include <asm/memory.h>

#ifdef CONFIG_DOMAIN_PAGE

struct mapcache mapcache;

extern pte_t *mapcache_table_vector;

void *map_domain_page(unsigned long pfn)
{
	unsigned int idx, i;
	unsigned long va;

	ASSERT(!in_irq());

	spin_lock(&mapcache.lock);

	idx = find_next_zero_bit(mapcache.inuse, MAPCACHE_ENTRIES, mapcache.cursor);
	if ( unlikely(idx >= MAPCACHE_ENTRIES) )
	{
		
		/* /First/, clean the garbage map and update the inuse list. */
		for ( i = 0; i < ARRAY_SIZE(mapcache.garbage); i++ )
		{
			unsigned long x = xchg(&mapcache.garbage[i], 0);
			mapcache.inuse[i] &= ~x;
		}

		idx = find_first_zero_bit(mapcache.inuse, MAPCACHE_ENTRIES);
		ASSERT(idx < MAPCACHE_ENTRIES);
	}

	set_bit(idx, mapcache.inuse);
	mapcache.cursor = idx + 1;

	spin_unlock(&mapcache.lock);

	consistent_write(&mapcache.table[idx], ((pfn << PAGE_SHIFT) | PTE_TYPE_SMALL | PTE_BUFFERABLE | PTE_CACHEABLE | PTE_SMALL_AP_UNO_SRW));

	va = MAPCACHE_VIRT_START + (idx << PAGE_SHIFT);
	cpu_flush_tlb_entry(va, TLBF_ITLB | TLBF_DTLB);

	return (void *)va;
}

void unmap_domain_page(void *va)
{
	unsigned int idx;

	ASSERT((void *)MAPCACHE_VIRT_START <= va);
	ASSERT(va < (void *)MAPCACHE_VIRT_END);

	idx = ((unsigned long)va - MAPCACHE_VIRT_START) >> PAGE_SHIFT;
	
	consistent_write(&mapcache.table[idx], 0);
	cpu_flush_tlb_entry((unsigned long)va, TLBF_ITLB | TLBF_DTLB);

	set_bit(idx, mapcache.garbage);
}

#endif

static __init int mapcache_init(void)
{
    unsigned int i, j;
    unsigned long pgd_idx;

    mapcache.table = mapcache_table_vector;

    spin_lock_init(&mapcache.lock);

    /* Mark all maphash entries as not in use. */
    for ( i = 0; i < MAX_VIRT_CPUS; i++ )
        for ( j = 0; j < MAPHASH_ENTRIES; j++ )
            mapcache.vcpu_maphash[i].hash[j].idx = MAPHASHENT_NOTINUSE;

    return 0;
}

__initcall(mapcache_init);

