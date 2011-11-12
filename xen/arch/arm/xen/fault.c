/*
 * fault.c
 *
 * Copyright (C) 2008-2011 Samsung Electronics
 *          Sang-bum Suh <sbuk.suh@samsung.com>
 *          JaeMin Ryu   <jm77.ryu@samsung.com>
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
#include <xen/types.h>
#include <xen/errno.h>
#include <xen/spinlock.h>
#include <asm/config.h>
#include <xen/lib.h>
#include <asm/irq.h>
#include <xen/types.h>
#include <xen/init.h>
#include <xen/errno.h>
#include <xen/spinlock.h>
#include <public/arch-arm.h>
#include <xen/bitmap.h>
#include <xen/sched.h>
#include <xen/event.h>
#include <xen/linkage.h>
#include <asm/hardirq.h>
#include <asm/uaccess.h>
#include <asm/string.h>
#include <asm/memmap.h>
#include <asm/dma.h>
#include <public/xen.h>
#include <xen/iocap.h>
#include <asm/current.h>

long do_set_alignment(long enable)
{
	unsigned int val;
	
	val = get_cr();
	
	if(enable)
		val |= CR_A;
	else
		val &= ~(CR_A);

	set_cr(val);
	return 0;
}
