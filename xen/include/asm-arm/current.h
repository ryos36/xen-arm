/*
 *  current.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *	ChanJu Park <beastworld@samsung.com>
 *	JaeMin Ryu  <jm77.ryu@samsung.com>
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
#ifndef __ARM_CURRENT_H__
#define __ARM_CURRENT_H__

#include <asm/page.h>

#ifndef __ASSEMBLY__
struct vcpu;

struct cpu_ctx {
	unsigned long	r0;
	unsigned long	r1;
	unsigned long	r2;
	unsigned long	r3;
	unsigned long	r4;
	unsigned long	r5;
	unsigned long	r6;
	unsigned long	r7;
	unsigned long	r8;
	unsigned long	r9;
	unsigned long	r10;
	unsigned long	r11;
	unsigned long	r12;
	unsigned long	usp;
	unsigned long	ulr;
	unsigned long	ssp;
	unsigned long	slr;
	unsigned long	pc;
	unsigned long	spsr;
	unsigned long	extra;
};

struct cpu_info {
	struct vcpu	*vcpu;
	unsigned long	vspsr;
	unsigned long	vsp;
	unsigned long	vlr;
	unsigned long	vdacr;
#ifdef CONFIG_PREEMPT
	unsigned long	preempt_count;
#endif
};

static inline struct cpu_info * current_cpu_info(void)
{
	register unsigned long sp asm("r13");
	return (struct cpu_info *) ( sp & ~(STACK_SIZE -1)  ); 
}

static inline struct vcpu *get_current(void)
{
        return current_cpu_info()->vcpu;
}

#define current get_current()

static inline void set_current(struct vcpu *v)
{   
    current_cpu_info()->vcpu = v;
}

static inline void set_current_vcpu(struct vcpu *v)
{
        struct cpu_info *ci;

        ci = current_cpu_info();
        ci->vcpu = v;
}

static inline void cpu_info_init(struct cpu_info *cpu_info)
{
        cpu_info->vcpu = NULL;
}

/* [CHECK] : 
 *
 */
#define guest_cpu_user_regs()	(&current->arch.ctx)
#endif

#endif /* __ARM_CURRENT_H__ */
