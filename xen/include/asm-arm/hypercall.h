/*
 * hypercall.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *          JooYoung Hwang <jooyoung.hwang@samsung.com>
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

#ifndef __ARM_HYPERCALL_H__
#define __ARM_HYPERCALL_H__

#include <public/physdev.h>

extern long do_set_trap_table(XEN_GUEST_HANDLE(trap_info_t) traps);

extern int do_mmu_update(XEN_GUEST_HANDLE(mmu_update_t) ureqs,
			 unsigned int count,
			 XEN_GUEST_HANDLE(uint) pdone,
			 unsigned int foreigndom);

extern long do_set_gdt(XEN_GUEST_HANDLE(ulong) frame_list,
		       unsigned int entries);

extern long do_stack_switch(unsigned long ss, unsigned long esp);

extern long do_fpu_taskswitch(int set);

extern long do_set_debugreg(int reg, unsigned long value);

extern unsigned long do_get_debugreg(int reg);

extern long do_update_descriptor(u64 pa, u64 desc);

extern int do_update_va_mapping(u32 va, u32 flags, u64 val64);

extern long do_physdev_op(XEN_GUEST_HANDLE(physdev_op_t) uop);

extern int do_update_va_mapping_otherdomain(unsigned long va,
					    u64 val64,
					    unsigned long flags,
					    domid_t domid);

extern int do_mmuext_op(XEN_GUEST_HANDLE(mmuext_op_t) uops,
			unsigned int count,
			XEN_GUEST_HANDLE(uint) pdone,
			unsigned int foreigndom);

extern unsigned long do_iret(void);

struct vcpu;
extern long arch_do_vcpu_op(int cmd, struct vcpu *v, XEN_GUEST_HANDLE(void) arg);

extern long do_set_callbacks(unsigned long event, unsigned long failsafe);
#endif /* __ARM_HYPERCALL_H__ */
