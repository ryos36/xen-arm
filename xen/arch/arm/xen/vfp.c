/*
 * vfp.c
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
#include <xen/lib.h>
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

#define FPEXC           cr8
#define FPINST          cr9
#define FPINST2         cr10
#define MVFR1			cr6
#define MVFR0			cr7

#define vfpreg(_vfp_) #_vfp_

#define fmrx(_vfp_) 					\
({          							\
		u32 __v;            			\
		asm("mrc p10, 7, %0, " vfpreg(_vfp_) ", cr0, 0 @ fmrx   %0, " #_vfp_    \
			: "=r" (__v) : : "cc"); \
		__v;                \
})

#define fmxr(_vfp_,_var_)       \
		asm("mcr p10, 7, %0, " vfpreg(_vfp_) ", cr0, 0 @ fmxr   " #_vfp_ ", %0" \
		: : "r" (_var_) : "cc")

asmlinkage long do_vfp_op(unsigned int cmd, unsigned int cpnum, unsigned long val)
{
	if(cmd == CMD_FMRX){
		switch(cpnum){
			case FPEXC_XEN:
				return fmrx(FPEXC);
			case FPINST_XEN:
				return fmrx(FPINST);
			case FPINST2_XEN:
				return fmrx(FPINST2);
			case MVFR0_XEN:
				return fmrx(MVFR0);
			default:
				break;
		}
	} else if(cmd == CMD_FMXR){
			switch(cpnum){
				case FPEXC_XEN:
					fmxr(FPEXC,val);
					return val;
				case FPINST_XEN:
					fmxr(FPINST,val);
					return val;
				case FPINST2_XEN:
					fmxr(FPINST2,val);
					return val;
				default:
					break;
		}
	}

	return -EPERM;
}

asmlinkage long do_set_tls(unsigned long val)
{
	asm ("mcr p15, 0, %0, c13, c0, 3" : :
			"r" (val));

	return val;
}

