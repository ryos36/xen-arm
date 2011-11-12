/*
 * physdev.c
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

#include <xen/config.h>
#include <xen/linkage.h>
#include <xen/lib.h>
#include <xen/types.h>
#include <xen/init.h>
#include <xen/errno.h>
#include <xen/spinlock.h>
#include <xen/bitmap.h>
#include <xen/sched.h>
#include <xen/event.h>
#include <xen/config.h>

#include <security/acm/acm_hooks.h>

#include <public/arch-arm.h>
#include <public/physdev.h>

#include <asm/linkage.h>	
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/coprocessor.h>

extern int pirq_guest_unmask(struct domain *d);

asmlinkage long do_physdev_op(struct physdev_op *uop)
{
	struct physdev_op op;
	long ret;
	int irq;

	if ( unlikely(copy_from_user(&op, uop, sizeof(struct physdev_op)) != 0) )
		return -EFAULT;
	
	switch ( op.cmd ) {
		case PHYSDEVOP_SET_IRQ_TYPE:
			ret = set_irq_type(op.u.set_irq_type_op.irq, op.u.set_irq_type_op.flags);
			break;
		case PHYSDEVOP_EOI:
			ret = pirq_guest_eoi(current->domain, op.u.eoi_op.irq);
			break;
		case PHYSDEVOP_IRQ_UNMASK_NOTIFY:
			while(1);

//			ret = pirq_guest_unmask(current->domain);
 			break;
		case PHYSDEVOP_IRQ_STATUS_QUERY:
			irq = op.u.irq_status_query.irq;
			ret = -EINVAL;
			if ( (irq < 0) || (irq >= NR_IRQS) )
				break;

			if(!acm_irq_status_query(irq)){
				ret = -EPERM;
				break;
			}

			op.u.irq_status_query.flags = 0;
#if 0
			/* Edge-triggered interrupts don't need an explicit unmask downcall. */
			if (interrupt_descriptors[irq].flags & IRQT_LEVEL) 
				op.u.irq_status_query.flags |= PHYSDEVOP_IRQ_NEEDS_UNMASK_NOTIFY;
#else
			if (!strstr(irq_desc[irq].chip->trigger_type, "edge") )
				op.u.irq_status_query.flags |= PHYSDEVOP_IRQ_NEEDS_UNMASK_NOTIFY;
#endif
			ret = 0;
			break;
		case PHYSDEVOP_AUX:
			{
				unsigned long flags;
				local_irq_save(flags);
				if(op.u.aux_op.vector)
					MCR(p15, 0, op.u.aux_op.aux, c1, c0, 1);
				else {
					MRC(p15, 0, ret, c1, c0, 1);
				}
			 	local_irq_restore(flags);
			}
			break;	
		default:
			ret = -EINVAL;
			break;
	}

	if ( copy_to_user(uop, &op, sizeof(struct physdev_op)) )
		ret = -EFAULT;

	return ret;
}
