/*
 * cache.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *          JaeMin Ryu  <jm77.ryu@samsung.com>
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

#ifndef __ARM_CACHE_H__
#define __ARM_CACHE_H__

#include <xen/prefetch.h>

#if 0
#define CACHE_TYPE(x)		(((x) >> 25) & 15)
#define CACHE_S(x)		((x) & (1 << 24))
#define CACHE_DSIZE(x)		(((x) >> 12) & 4095)    /* only if S=1 */
#define CACHE_ISIZE(x)		((x) & 4095)

#define CACHE_SIZE(y)		(((y) >> 6) & 7)
#define CACHE_ASSOCIATIVITY(y)	(((y) >> 3) & 7)
#define CACHE_M(y)		((y) & (1 << 2))
#define CACHE_LINE(y)		((y) & 3)
#endif

#define L1_CACHE_BYTES		32

#define CACHE_VIVT		(0x00000001)
#define CACHE_VIPT		(0x00000002)
#define CACHE_PIPT		(0x00000004)

#define INVL_ICACHE
#define INVL_DCACHE


#define CLIDR_LOU_MASK		(0x3 << 27)
#define CLIDR_LOC_MASK		(0x3 << 24)
#define CLIDR_CTYPE8_MASK	(0x3 << 21)
#define CLIDR_CTYPE7_MASK	(0x3 << 18)
#define CLIDR_CTYPE6_MASK	(0x3 << 15)
#define CLIDR_CTYPE5_MASK	(0x3 << 12)
#define CLIDR_CTYPE4_MASK	(0x3 << 9)
#define CLIDR_CTYPE3_MASK	(0x3 << 6)
#define CLIDR_CTYPE2_MASK	(0x3 << 3)
#define CLIDR_CTYPE1_MASK	(0x3 << 0)

#define CTYPE_VAL_NOCACHE	(0)
#define CTYPE_VAL_ICACHE_ONLY	(1)
#define CTYPE_VAL_DCACHE_ONLY	(2)
#define CTYPE_VAL_HAVARD_CACHE	(3)
#define CTYPE_VAL_UNIFIED_CACHE	(4)


#ifndef __ASSEMBLY__
static inline void cache_flush(unsigned long start, unsigned long end, unsigned int flags)
{
}

static inline void cache_invalidate(unsigned long start, unsigned long end, unsigned int flags)
{
}

static inline void cache_clean(unsigned long start, unsigned long end, unsigned int flags)
{
}

static inline void dump_cache(const char *prefix, int cpu, unsigned int cache)
{
}

static inline void lock_icache_line(unsigned long line)
{
}

static inline void lock_dcache_line(unsigned long line)
{
}

static inline void unlock_icache_line(unsigned long line)
{
}

static inline void unlock_dcache_line(unsigned long line)
{
}

#endif
#endif
