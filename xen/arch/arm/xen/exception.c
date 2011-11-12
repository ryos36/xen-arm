/*
 * exception.c
 *
 * Copyright (C) 2008-2011 Samsung Electronics
 *          Sang-bum Suh <sbuk.suh@samsung.com>
 *          Jaemin Ryu   <jm77.ryu@samsung.com>
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
#include <asm/current.h>
#include <asm/uaccess.h>

extern struct extable_entry _sextable[];
extern struct extable_entry _eextable[];
extern struct extable_entry _spreextable[];
extern struct extable_entry _epreextable[];

static void __init sort_extable(struct extable_entry *start, struct extable_entry *end)
{
	struct extable_entry *p, *q, tmp;

	for ( p = start; p < end; p++ )
	{
		for ( q = p-1; q > start; q-- )
			if ( p->instr > q->instr )
				break;
			if ( ++q != p )
			{
				tmp = *p;
				memmove(q+1, q, (p-q)*sizeof(*p));
				*q = tmp;
			}
	}
}

void __init sort_extables(void)
{
	sort_extable(_sextable, _eextable);
	sort_extable(_spreextable, _epreextable);
}

static inline unsigned long search_one_table(const struct extable_entry *first, const struct extable_entry *last, unsigned long value)
{
	const struct extable_entry *mid;
	long diff;

	while ( first <= last )
	{
		mid = (last - first) / 2 + first;
		diff = mid->instr - value;
		if (diff == 0)
			return mid->fixup;
		else if (diff < 0)
			first = mid+1;
		else
			last = mid-1;
	}

	return 0;
}

unsigned long search_extable(unsigned long addr)
{
	return search_one_table(_sextable, _eextable - 1, addr);
}

unsigned long search_pre_extable(struct cpu_ctx *regs)
{
	unsigned long addr = (unsigned long)regs->pc;
	unsigned long fixup = search_one_table(_spreextable, _epreextable-1, addr);

	if ( fixup )
	{
		printk("Pre-exception: %p -> %p\n", _p(addr), _p(fixup));
	}

	return fixup;
}

