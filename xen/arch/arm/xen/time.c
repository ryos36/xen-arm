/*
 * time.c 
 *
 * Copyright (C) 2008-2011 Samsung Electronics 
 *          Sang-bum Suh   <sbuk.suh@samsung.com>
 *          JooYoung Hwang <jooyoung.hwang@samsung.com>
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
#include <xen/time.h>
#include <xen/sched.h>
#include <xen/event.h>
#include <xen/softirq.h>
#include <asm/types.h>
#include <asm/current.h>
#include <asm/div64.h>
#include <asm/time.h>

#define INITIAL_JIFFIES ((unsigned long)(unsigned int)(0UL));

u64 jiffies64 __cacheline_aligned = INITIAL_JIFFIES;

unsigned long loops_per_jiffy = (1 << 12);

void timer_tick(unsigned int ticks)
{
	struct vcpu *v   = current;
	unsigned int cpu = smp_processor_id();

	jiffies64 += ticks;

#if defined(CONFIG_SMP)
	smp_send_timer();
#endif

	raise_softirq(TIMER_SOFTIRQ);
}


void update_vcpu_system_time(struct vcpu *v)
{
	return;
}

/* Set clock to <secs,usecs> after 00:00:00 UTC, 1 January, 1970. */
void do_settime(unsigned long secs, unsigned long nsecs, u64 system_time_base)
{
}

