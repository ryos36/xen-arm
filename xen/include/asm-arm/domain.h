/*
 * domain.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *          JooYoung Hwang <jooyoung.hwang@samsung.com>
 *          Jaemin Ryu <jm77.ryu@samsung.com>
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


#ifndef __ARM_DOMAIN_H__
#define __ARM_DOMAIN_H__

#include <asm/mm.h>
#include <asm/page.h>
#include <asm/current.h>
#include <xen/cpumask.h>
#include <xen/cache.h>
#include <xen/spinlock.h>
#include <public/arch-arm.h>

#define MAPHASH_ENTRIES			8
#define MAPHASH_HASHFN(pfn)		((pfn) & (MAPHASH_ENTRIES-1))
#define MAPHASHENT_NOTINUSE		((u16)~0U)

struct vcpu_maphash {
    struct vcpu_maphash_entry {
        unsigned long pfn;
        uint16_t      idx;
        uint16_t      refcnt;
    } hash[MAPHASH_ENTRIES];
}__cacheline_aligned;


#define MAPCACHE_ORDER   8
#define MAPCACHE_ENTRIES (1 << MAPCACHE_ORDER)

struct mapcache {
    /* The PTEs that provide the mappings, and a cursor into the array. */
    pte_t	*table;
    unsigned int cursor;

    /* Protects map_domain_page(). */
    spinlock_t lock;

    /* Which mappings are in use, and which are garbage to reap next epoch? */
    unsigned long inuse[BITS_TO_LONGS(MAPCACHE_ENTRIES)];
    unsigned long garbage[BITS_TO_LONGS(MAPCACHE_ENTRIES)];

    /* Lock-free per-VCPU hash of recently-used mappings. */
    struct vcpu_maphash vcpu_maphash[MAX_VIRT_CPUS];
}__cacheline_aligned;

struct arch_domain
{
    /* I/O-port admin-specified access capabilities. */
    struct rangeset	*ioport_caps;
};

struct arch_vcpu
{
	struct vcpu_guest_context ctx;
} __cacheline_aligned;

void startup_cpu_idle_loop(void);

#endif 

