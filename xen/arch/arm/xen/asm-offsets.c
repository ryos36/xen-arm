/*
 * Copyright (C) 1995-2003 Russell King
 *               2001-2002 Keith Owens
 *
 * Generate definitions needed by assembly language modules.
 * This code generates raw asm output which is post-processed to extract
 * and format the required data.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <xen/config.h>
#include <xen/mm.h>
#include <xen/perfc.h>
#include <xen/sched.h>
#include <xen/irq_cpustat.h>
#include <asm/current.h>

#if defined(__APCS_26__)
#error Sorry, your compiler targets APCS-26 but this kernel requires APCS-32
#endif
/*
 * GCC 2.95.1, 2.95.2: ignores register clobber list in asm().
 * GCC 3.0, 3.1: general bad code generation.
 * GCC 3.2.0: incorrect function argument offset calculation.
 * GCC 3.2.x: miscompiles NEW_AUX_ENT in fs/binfmt_elf.c
 *            (http://gcc.gnu.org/PR8896) and incorrect structure
 *	      initialisation in fs/jffs2/erase.c
 */
#if __GNUC__ < 2 || \
   (__GNUC__ == 2 && __GNUC_MINOR__ < 95) || \
   (__GNUC__ == 2 && __GNUC_MINOR__ == 95 && __GNUC_PATCHLEVEL__ != 0 && \
					     __GNUC_PATCHLEVEL__ < 3) || \
   (__GNUC__ == 3 && __GNUC_MINOR__ < 3)
#error Your compiler is too buggy; it is known to miscompile kernels.
#error    Known good compilers: 2.95.3, 2.95.4, 2.96, 3.3
#endif

/* Use marker if you need to separate the values later */

#define DEFINE(sym, val) \
        asm volatile("\n->" #sym " %0 " #val : : "i" (val))

#define BLANK() asm volatile("\n->" : : )

int main(void)
{
	DEFINE(OFFSET_SOFTIRQ_PENDING,		offsetof(struct irq_cpu_stat, __softirq_pending));
	DEFINE(OFFSET_LOCAL_IRQ_COUNT,		offsetof(struct irq_cpu_stat, __local_irq_count));
	DEFINE(OFFSET_NMI_COUNT,		offsetof(struct irq_cpu_stat, __nmi_count));
	DEFINE(SIZE_IRQ_CPU_STAT,		sizeof(struct irq_cpu_stat));
	BLANK();
	DEFINE(OFFSET_VCPU_INFO,		offsetof(struct vcpu, vcpu_info));
	DEFINE(OFFSET_ARCH_VCPU,		offsetof(struct vcpu, arch));
	BLANK();
	//DEFINE(OFFSET_EVTCHN_UPCALL_MASK,	offsetof(struct vcpu_info, evtchn_upcall_mask));
	DEFINE(OFFSET_EVTCHN_UPCALL_PENDING,	offsetof(struct vcpu_info, evtchn_upcall_pending));
	DEFINE(OFFSET_ARCH_VCPU_INFO,		offsetof(struct vcpu_info, arch));
	DEFINE(OFFSET_TSP,			offsetof(struct arch_vcpu_info, sp));
	DEFINE(OFFSET_TLR,			offsetof(struct arch_vcpu_info, lr));
	DEFINE(OFFSET_TCPSR,			offsetof(struct arch_vcpu_info, cpsr));
	DEFINE(OFFSET_TSPSR,			offsetof(struct arch_vcpu_info, spsr));
	DEFINE(OFFSET_VCR,			offsetof(struct arch_vcpu_info, cr));
	DEFINE(OFFSET_VDACR,			offsetof(struct arch_vcpu_info, dacr));
	DEFINE(OFFSET_VCPAR,			offsetof(struct arch_vcpu_info, cpar));
	DEFINE(OFFSET_VPIDR,			offsetof(struct arch_vcpu_info, pidr));
	DEFINE(OFFSET_VFSR,			offsetof(struct arch_vcpu_info, fsr));
	DEFINE(OFFSET_VFAR,			offsetof(struct arch_vcpu_info, far));
	BLANK();
	DEFINE(OFFSET_GUEST_CONTEXT,		offsetof(struct arch_vcpu, ctx));
	DEFINE(OFFSET_VECTOR_RESET,		0);
	DEFINE(OFFSET_VECTOR_UND,		4);
	DEFINE(OFFSET_VECTOR_SWI,		8);
	DEFINE(OFFSET_VECTOR_PABT,		12);
	DEFINE(OFFSET_VECTOR_DABT,		16);
	DEFINE(OFFSET_VECTOR_IRQ,		24);
	DEFINE(OFFSET_VECTOR_FIQ,		28);
	BLANK();
	DEFINE(OFFSET_VCPU,			offsetof(struct cpu_info, vcpu));
	DEFINE(OFFSET_VPSR,			offsetof(struct cpu_info, vspsr));
	DEFINE(OFFSET_VSP,			offsetof(struct cpu_info, vsp));
	DEFINE(OFFSET_VLR,			offsetof(struct cpu_info, vlr));
	BLANK();
	DEFINE(OFFSET_VCPU_R0,			offsetof(struct vcpu_guest_context, r0));
	DEFINE(OFFSET_VCPU_R1,			offsetof(struct vcpu_guest_context, r1));
	DEFINE(OFFSET_VCPU_R2,			offsetof(struct vcpu_guest_context, r2));
	DEFINE(OFFSET_VCPU_R3,			offsetof(struct vcpu_guest_context, r3));
	DEFINE(OFFSET_VCPU_R4,			offsetof(struct vcpu_guest_context, r4));
	DEFINE(OFFSET_VCPU_R5,			offsetof(struct vcpu_guest_context, r5));
	DEFINE(OFFSET_VCPU_R6,			offsetof(struct vcpu_guest_context, r6));
	DEFINE(OFFSET_VCPU_R7,			offsetof(struct vcpu_guest_context, r7));
	DEFINE(OFFSET_VCPU_R8,			offsetof(struct vcpu_guest_context, r8));
	DEFINE(OFFSET_VCPU_R9,			offsetof(struct vcpu_guest_context, r9));
	DEFINE(OFFSET_VCPU_R10,			offsetof(struct vcpu_guest_context, r10));
	DEFINE(OFFSET_VCPU_R11,			offsetof(struct vcpu_guest_context, r11));
	DEFINE(OFFSET_VCPU_R12,			offsetof(struct vcpu_guest_context, r12));
	DEFINE(OFFSET_VCPU_R13,			offsetof(struct vcpu_guest_context, r13));
	DEFINE(OFFSET_VCPU_R14,			offsetof(struct vcpu_guest_context, r14));
	DEFINE(OFFSET_VCPU_R15,			offsetof(struct vcpu_guest_context, r15));
	DEFINE(OFFSET_VCPU_DACR,		offsetof(struct vcpu_guest_context, dacr));
	DEFINE(OFFSET_VCPU_VBAR,		offsetof(struct vcpu_guest_context, vbar));
	DEFINE(OFFSET_VCPU_CONTEXTIDR,		offsetof(struct vcpu_guest_context, contextidr));
	DEFINE(OFFSET_VCPU_FCSEIDR,		offsetof(struct vcpu_guest_context, fcseidr));
	DEFINE(OFFSET_VCPU_TTBR0,		offsetof(struct vcpu_guest_context, ttbr0));
	DEFINE(OFFSET_VCPU_TTBR1,		offsetof(struct vcpu_guest_context, ttbr1));
	DEFINE(OFFSET_VCPU_TTBCR,		offsetof(struct vcpu_guest_context, ttbcr));
	DEFINE(OFFSET_HYPERVISOR_CALLBACK,	offsetof(struct vcpu_guest_context, event_callback));
	DEFINE(OFFSET_FAILSAFE_CALLBACK,	offsetof(struct vcpu_guest_context, failsafe_callback));
	BLANK();

	return 0; 
}
