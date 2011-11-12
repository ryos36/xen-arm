/*
 * hypercalls.S
 *
 * Copyright (C) 2008-2011 Samsung Electronics
 *          Sang-bum Suh <sbuk.suh@samsung.com>
 *          JaeMin Ryu   <jm77.ryu@samsung.com>
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
#include <xen/linkage.h>
#include <asm/page.h>
#include <asm/system.h>
#include <asm/cpu-domain.h>
#include <asm/asm-offsets.h>
#include <asm/asm-macros.h>
#include <asm/arch/entry-macro.S>

#include <public/arch-arm.h>

#ifndef CONFIG_VMM_SECURITY_ACM	
ENTRY(do_acm_op)
	mov	pc, lr
#endif	

#define TLBF_INVL_ITLB		1
#define TLBF_INVL_DTLB		2
#define ILFB_DRAIN_WB		4
#define PAGE_SZ			PAGE_SIZE

ENTRY(do_set_domain)
	mov	pc, lr

ENTRY(do_iwmmxt_op)
	mrc	p15, 0, r1, c15, c1, 0
	cmp	r0, #2
	beq	1f

	mov	r2, r1
	cmp	r0, #0
	orreq	r2, r2, #0x3
	cmp	r0, #1
	biceq	r2, r2, #0x03
	mcr	p15, 0, r2, c15, c1, 0

	cpwait	r2
1:
	mov	r0, r1
	mov	pc, lr

ENTRY(do_cache_control)
	b 	.
	mov 	r3, #0
	mcr 	p15, 0, r3, c7, c10, 4      @ drain WB
	bic 	r0, r0, #0x0ff
	bic 	r0, r0, #0xf00
1:  tst		r2, #TLBF_INVL_ITLB
	mcrne 	p15, 0, r0, c8, c5, 1       @ invalidate I TLB entry
	mcr 	p15, 0, r0, c8, c6, 1       @ invalidate D TLB entry
	add 	r0, r0, #PAGE_SZ
	cmp	r0, r1
	blo 	1b
	mov 	pc, lr

ENTRY(do_sync_vcpu_state)
	cci	r8
	ldr	r4, [r8, #OFFSET_VCPU]
	ldr	r11, [r4, #OFFSET_VCPU_INFO]
	ldr	r3, [r11, #(OFFSET_ARCH_VCPU_INFO + OFFSET_VCPAR)]
	orr	r3, r3, #0x40
	orr	r3, r3, #(1 << 13)
	mcr	p15, 0, r3, c15, c1, 0
	mrc	p15, 0, r2, c2, c0, 0
	mov	r2, r2
	mov	pc, lr

ENTRY(do_restore_trap_frame)
	cci	r8
	ldr	r4, [r8, #OFFSET_VCPU]
	ldr	r6, [sp, #CTXT_USP]
	ldr	r11, [r4, #OFFSET_VCPU_INFO]

	ldr     r3, [r11, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TSPSR)]
	ldr     r2, [r11, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TLR)]
	ldr     r1, [r11, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TSP)]

	ldr	r7, [r11, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TCPSR)]

	tst	r3, #PSR_I_BIT
	orrne	r7, #VPSR_I_BIT
	biceq	r7, #VPSR_I_BIT

	bic	r5, r3, #(PSR_MODE_MASK | PSR_I_BIT)
	orr	r5, r5, #PSR_MODE_USR
	and	r3, r3, #PSR_MODE_MASK

	@ Construct latest guest context
	str	r1, [sp, #CTXT_USP]
	str	r2, [sp, #CTXT_PC]
	str	r5, [sp, #CTXT_SPSR]
	str	r3, [r8, #4]
	str	r6, [r8, #8]

	@ Update VPSR
	str    r7, [r11, #(OFFSET_ARCH_VCPU_INFO + OFFSET_TCPSR)]

	mov	pc, lr
