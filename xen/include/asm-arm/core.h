/*
 * core.h
 *
 * Copyright (C) 2008 Samsung Electronics
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

#ifndef __ARM_CORE_H__
#define __ARM_CORE_H__

#define CPUID_ID		0
#define CPUID_CACHE_TYPE	1
#define CPUID_TCM		2
#define CPUID_TLB_TYPE		3

#define read_cpuid(reg)					\
({							\
	unsigned int val;				\
	asm("mrc p15, 0, %0, c0, c0, " STR(reg)		\
	    : "=r" (val)				\
	    :						\
	    : "cc");					\
	val;						\
})


int detect_core(void);
#endif /* __ARM_CORE_H__ */
