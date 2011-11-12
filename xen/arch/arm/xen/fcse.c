/*
 * fcse.c
 *
 * Copyright (C) 2008-2011 Samsung Electronics
 *          Sang-bum Suh <sbuk.suh@samsung.com>
 *          JaeMin Ryu   <jm77.ryu@samsung.com>
 *
 * Secure Xen on ARM architecture designed by Sang-bum Suh consists of
 * Xen on ARM and the associated access control.
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

#include <xen/config.h>
#include <xen/init.h>
#include <xen/lib.h>
#include <xen/spinlock.h>
#include <xen/list.h>
#include <xen/compiler.h>
#include <xen/types.h>
#include <xen/cpumask.h>
#include <asm/fcse.h>

#if 0
static unsigned long fcse_global_id_map[((NR_FCSE_IDS + BITS_PER_LONG - 1) / BITS_PER_LONG)];
static unsigned long fcse_global_cache_dirty_map[((NR_FCSE_IDS + BITS_PER_LONG - 1) / BITS_PER_LONG)];
#endif

void fcse_set_pid(unsigned long pid)
{
    __asm__ __volatile__ ("mcr p15, 0, %0, c13, c0, 0" : /* */: "r" (pid) : "memory");
}

unsigned long fcse_get_pid(void)
{
	unsigned long pid;
    __asm__ __volatile__("mrc p15, 0, %0, c13, c0, 0" : "=&r" (pid));

	return pid;
}

static __init int fcse_init(void)
{
	printf("Initializing ARM FCSE Unit\n");

	return 0;
}
__initcall(fcse_init);

