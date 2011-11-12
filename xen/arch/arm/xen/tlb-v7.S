/*
 *  linux/arch/arm/mm/tlb-v7.S
 *
 *  Copyright (C) 1997-2002 Russell King
 *  Modified for ARMv7 by Catalin Marinas
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  ARM architecture version 6 TLB handling functions.
 *  These assume a split I/D TLB.
 */

#include <xen/config.h>
#include <xen/linkage.h>
#include <asm/page.h>
#include <asm/cpu-ops.h>

#include <public/arch-arm.h>

#define PAGE_SZ 4096 /* PAGE_SIZE	@ */

/*---------------------------------------------------------------------------
 * r0 : TLB Flags
 *--------------------------------------------------------------------------*/
PRIVATE(v7_flush_tlb_all)
	dsb
	mov	ip,	#0
#ifndef CONFIG_SMP
	tst	r0, #TLBF_ITLB
	mcrne	p15, 0, ip, c8, c6,  0		@ invalidate Entire I TLB
	tst	r0, #TLBF_DTLB
	mcrne	p15, 0, ip, c8, c5,  0		@ invalidate Entire D TLB
#else
	mcr	p15, 0, ip, c8, c3, 0
#endif
	mcr	p15, 0, ip, c7, c5, 6		@ flush BTAC/BTB (shareable)
	dsb
	isb
	mov	pc, lr

DECLARE_CPU_OP(cpu_flush_tlb_all, v7_flush_tlb_all)

/*---------------------------------------------------------------------------
 * r0 : Entry
 * r1 : TLB Flags
 *--------------------------------------------------------------------------*/
PRIVATE(v7_flush_tlb_entry)
	dsb
#ifndef CONFIG_SMP
	tst	r1, #TLBF_DTLB
	mcrne	p15, 0, r0, c8, c6, 1		@ TLB invalidate D MVA
	tst	r1, #TLBF_ITLB
	mcrne	p15, 0, r0, c8, c5, 1		@ TLB invalidate I MVA
#else
	mcr	p15, 0, r0, c8, c3, 1		@ TLB invalidate U MVA (shareable) 
#endif
	mov	ip, #0
	mcr	p15, 0, ip, c7, c5, 6		@ flush BTAC/BTB (shareable)
	dsb
	mov	pc, lr

DECLARE_CPU_OP(cpu_flush_tlb_entry, v7_flush_tlb_entry)

/*---------------------------------------------------------------------------
 * r0 : start
 * r1 : end
 * r2 : flags
 *--------------------------------------------------------------------------*/
PRIVATE(v7_flush_tlb_range)
	dsb	
1:
#ifndef CONFIG_SMP
	tst	r2, #TLBF_DTLB
	mcrne	p15, 0, r0, c8, c6, 1		@ TLB invalidate D MVA
	tst	r2, #TLBF_ITLB
	mcrne	p15, 0, r0, c8, c5, 1		@ TLB invalidate I MVA
#else
	mcr	p15, 0, r0, c8, c3, 1		@ TLB invalidate U MVA (shareable) 
#endif
	add	r0, r0, #PAGE_SZ
	cmp	r0, r1
	blo	1b
	mov	ip, #0
	mcr	p15, 0, ip, c7, c5, 6		@ flush BTAC/BTB (shareable)
	dsb
	isb
	mov	pc, lr
	
DECLARE_CPU_OP(cpu_flush_tlb_range, v7_flush_tlb_range)


