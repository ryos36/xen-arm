/*
 * tlb.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *          SungKwan Heo  <sk.heo@samsung.com>
 *          JaeMin Ryu    <jm77.ryu@samsung.com>
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
#ifndef __ARM_TLB_H__
#define __ARM_TLB_H__

#include <xen/config.h>
#include <xen/smp.h>

#include <asm/cpu-ops.h>

#define TLB_V3_PAGE		(1 << 0)
#define TLB_V4_U_PAGE		(1 << 1)
#define TLB_V4_D_PAGE		(1 << 2)
#define TLB_V4_I_PAGE		(1 << 3)
#define TLB_V6_U_PAGE		(1 << 4)
#define TLB_V6_D_PAGE		(1 << 5)
#define TLB_V6_I_PAGE		(1 << 6)

#define TLB_V3_FULL		(1 << 8)
#define TLB_V4_U_FULL		(1 << 9)
#define TLB_V4_D_FULL		(1 << 10)
#define TLB_V4_I_FULL		(1 << 11)
#define TLB_V6_U_FULL		(1 << 12)
#define TLB_V6_D_FULL		(1 << 13)
#define TLB_V6_I_FULL		(1 << 14)

#define TLB_V6_U_ASID		(1 << 16)
#define TLB_V6_D_ASID		(1 << 17)
#define TLB_V6_I_ASID		(1 << 18)

#define TLB_DCLEAN		(1 << 30)
#define TLB_WB			(1 << 31)


#define TLB_CAP_UNIFIED		(1 << 0)
#define TLB_CAP_ASID		(1 << 1)

#define local_flush_tlb(mask)
#define flush_tlb_mask(mask)		local_flush_tlb()

static void inline local_flush_tlb_all(unsigned int flags)
{
	ASSERT(flags != 0);

	cpu_flush_tlb_all(flags);
}

static void inline local_flush_tlb_range(unsigned long start, unsigned long end, unsigned int flags)
{
	ASSERT(flags != 0);
	ASSERT(start <= end);

	cpu_flush_tlb_range(start, end, flags);
}

static inline void lock_tlb_entry(unsigned long entry)
{
}

static inline void unlock_tlb_entry(unsigned long entry)
{
}

#endif /* __ARM_TLB_H__ */
