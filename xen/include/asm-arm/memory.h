/*
 * memory.h
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

#ifndef __ARM_MEMORY_H__
#define __ARM_MEMORY_H__

#include <asm/config.h>

#ifdef ARCH_NR_BANKS
#define NR_BANKS	ARCH_NR_BANKS
#else
#define NR_BANKS	4
#endif

struct memory_bank {
	unsigned long	base;
	unsigned long	size;
	int		node;
};


struct meminfo {
	unsigned long nr_banks;
	struct memory_bank banks[NR_BANKS];
};

extern struct meminfo system_memory;
void register_memory_bank(unsigned long base, unsigned long size);

static inline void consistent_write(void *ptr, unsigned long value)
{
	__asm__ __volatile__(
		"str	%1, [%0]					\n"
		"nop							\n"
#if 0
		"bic	%0, %0, #0x1f					\n"
#endif
		"mcr	p15, 0, %0, c7, c10, 1		@ clean D entry	\n"
#if 1
		"mcr	p15, 0, %2, c7, c10, 4  	@ drain WB	\n"
#endif
		:
		: "r"(ptr), "r"(value), "r"(0)
		: "memory");
}

#define pte_sync(ptr)						\
do {								\
	__asm__ __volatile__(					\
	"mcr p15, 0, %0, c7, c10, 1 @ clean D entry	\n"	\
	:							\
	: "r"(ptr), "r"(0)					\
	: "memory");						\
}while(0)

#endif /* __ARM_MEMORY_H__ */

