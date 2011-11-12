/*
 * smp.c
 *
 * Copyright (C) 2008, 2009 Samsung Electronics
 *         Sang-bum Suh <sbuk.suh@samsung.com>
 *         Jaemin Ryu   <jm77.ryu@samsung.com>
 *
 * Secure Xen on ARM architecture designed by Sang-bum Suh consists of
 * Xen on ARM and the associated access control.
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
#include <xen/spinlock.h>
#include <xen/cpumask.h>
#include <xen/smp.h>

#include <asm/time.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/delay.h>

volatile int tegra250_core_map = 1;

static DEFINE_SPINLOCK(boot_lock);

int wakeup_cpu(unsigned int cpu)
{

	tegra250_core_map |= 1 <<  cpu;

	cpu_flush_cache_all();

	smp_cross_call(cpumask_of_cpu(cpu));

	return 0;
}

void platform_secondary_init(unsigned int cpu)
{
	gic_cpu_init(0, IO_ADDRESS(TEGRA_GIC_PROC_IF_BASE));
}
