/*
 *  linux/arch/arm/mm/proc-v7.S
 *
 *  Copyright (C) 2001 Deep Blue Solutions Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  This is the "shell" of the ARMv7 processor support.
 */

#include <xen/linkage.h>
#include <asm/page.h>
#include <asm/cpu-ops.h>
#include <asm/system.h>
#include <asm/asm-offsets.h>
#include <public/arch-arm.h>

#define TTB_S		(1 << 1)
#define TTB_RGN_NC	(0 << 3)
#define TTB_RGN_OC_WBWA	(1 << 3)
#define TTB_RGN_OC_WT	(2 << 3)
#define TTB_RGN_OC_WB	(3 << 3)
#define TTB_NOS		(1 << 5)
#define TTB_IRGN_NC	((0 << 0) | (0 << 6))
#define TTB_IRGN_WBWA	((0 << 0) | (1 << 6))
#define TTB_IRGN_WT	((1 << 0) | (0 << 6))
#define TTB_IRGN_WB	((1 << 0) | (1 << 6))

#define TTB_FLAGS	TTB_IRGN_WBWA|TTB_S|TTB_NOS|TTB_RGN_OC_WBWA

	.text
/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v7_idle)
	dsb
	wfi
	mov	pc, lr

DECLARE_CPU_OP(cpu_idle, v7_idle)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v7_halt)
	b	.

DECLARE_CPU_OP(cpu_halt, v7_halt)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v7_reset)
	mov	pc, r0

DECLARE_CPU_OP(cpu_reset, v7_reset)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v7_get_ttb)
	mrc	p15, 0, r0, c2, c0, 0		@ get the current TTB
	mov	pc, lr

DECLARE_CPU_OP(cpu_get_ttb, v7_get_ttb)

/*---------------------------------------------------------------------------
 * R0 : Translation table base address to be loaded.
 * R1 : MMU Flags
 *--------------------------------------------------------------------------*/
PRIVATE(v7_switch_mm)
	mov	r3, #0
	orr	r0, r0, #TTB_FLAGS
	mcr	p15, 0, r0, c2, c0, 0		@ set TTB 0

	tst	r1, #1
	mcrne	p15, 0, r3, c8, c3, 0		@ flush entire TLB
	mcrne	p15, 0, r3, c7, c5, 6		@ flush branch target cache
	dsb
	isb
	clrex
        mov     pc, lr

DECLARE_CPU_OP(cpu_switch_ttb, v7_switch_mm)

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


	.macro  dcache_line_size, reg, tmp
	mrc     p15, 1, \tmp, c0, c0, 0	@ read CSIDR
	and     \tmp, \tmp, #7		@ cache line size encoding
	mov     \reg, #16		@ size offset
	mov     \reg, \reg, lsl \tmp	@ actual cache line size
	.endm

	.macro v7_way_op, op
	dmb					@ ensure ordering with previous memory accesses
	mrc	p15, 1, r0, c0, c0, 1		@ read clidr
	ands	r3, r0, #0x7000000		@ extract loc from clidr
	mov	r3, r3, lsr #23			@ left align loc bit field
	beq	50f				@ if loc is 0, then no need to clean
	mov	r10, #0				@ start clean at cache level 0
10:
	add	r2, r10, r10, lsr #1		@ work out 3x current cache level
	mov	r1, r0, lsr r2			@ extract cache type bits from clidr
	and	r1, r1, #7			@ mask of the bits for current cache only
	cmp	r1, #2				@ see what cache we have at this level
	blt	40f				@ skip if no cache, or just i-cache
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	isb					@ isb to sych the new cssr&csidr
	mrc	p15, 1, r1, c0, c0, 0		@ read the new csidr
	and	r2, r1, #7			@ extract the length of the cache lines
	add	r2, r2, #4			@ add 4 (line length offset)
	ldr	r4, =0x3ff
	ands	r4, r4, r1, lsr #3		@ find maximum number on the way size
	clz	r5, r4				@ find bit position of way size increment
	ldr	r7, =0x7fff
	ands	r7, r7, r1, lsr #13		@ extract max number of the index size
20:
	mov	r9, r4				@ create working copy of max way size
30:
	orr	r11, r10, r9, lsl r5		@ factor way and cache number into r11
	orr	r11, r11, r7, lsl r2		@ factor index number into r11
	mcr	p15, 0, r11, c7, \op , 2	@ clean & invalidate by set/way
	subs	r9, r9, #1			@ decrement the way
	bge	30b
	subs	r7, r7, #1			@ decrement the index
	bge	20b
40:
	add	r10, r10, #2			@ increment cache number
	cmp	r3, r10
	bgt	10b
50:
	mov	r10, #0				@ swith back to cache level 0
	mcr	p15, 2, r10, c0, c0, 0		@ select current cache level in cssr
	dsb
	isb
	.endm
	.text
/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
PRIVATE(v7_flush_cache_all)
	stmfd	sp!, {r4-r5, r7, r9-r11, lr}
	v7_way_op c14
	mov	r0, #0
	mcr	p15, 0, r0, c7, c5, 0	@ I+BTB cache invalidate
	ldmfd	sp!, {r4-r5, r7, r9-r11, lr}
	mov	pc, lr

DECLARE_CPU_OP(cpu_flush_cache_all, v7_flush_cache_all)

/*---------------------------------------------------------------------------
 *
 *	v7_flush_cache_range(start, end, flags)
 *
 *	Flush a range of TLB entries in the specified address space.
 *
 *	- start - start address (may not be aligned)
 *	- end   - end address (exclusive, may not be aligned)
 *	- flags	- vm_area_struct flags describing address space
 *
 *	It is assumed that:
 *	- we have a VIPT cache.
 *--------------------------------------------------------------------------*/
PRIVATE(v7_flush_cache_range)
	dcache_line_size r2, r3
1:
	mcr	p15, 0, r0, c7, c14, 1		@ clean & invalidate D line / unified line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr

DECLARE_CPU_OP(cpu_flush_cache_range, v7_flush_cache_range)

/*---------------------------------------------------------------------------
 * R0 : 
 *--------------------------------------------------------------------------*/
PRIVATE(v7_clean_cache_range)
	dcache_line_size r2, r3
1:	mcr	p15, 0, r0, c7, c10, 1		@ clean D entry
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr

DECLARE_CPU_OP(cpu_clean_cache_range, v7_clean_cache_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(v7_dma_inv_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
#if 0
	tst	r0, r3
	bic	r0, r0, r3
	mcrne	p15, 0, r0, c7, c14, 1		@ clean & invalidate D / U line

	tst	r1, r3
	bic	r1, r1, r3
	mcrne	p15, 0, r1, c7, c14, 1		@ clean & invalidate D / U line
#else
	bic	r0, r0, r3
#endif
1:
	mcr	p15, 0, r0, c7, c6, 1		@ invalidate D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
	
DECLARE_CPU_OP(cpu_invalidate_dma_range, v7_dma_inv_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(v7_dma_clean_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c10, 1		@ clean D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
	
DECLARE_CPU_OP(cpu_clean_dma_range, v7_dma_clean_range)

/*---------------------------------------------------------------------------
 *
 *--------------------------------------------------------------------------*/
ENTRY(v7_dma_flush_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r0, r0, r3
1:
	mcr	p15, 0, r0, c7, c14, 1		@ clean & invalidate D / U line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	dsb
	mov	pc, lr
	
DECLARE_CPU_OP(cpu_flush_dma_range, v7_dma_flush_range)

/*---------------------------------------------------------------------------
 * R0 : start address of region
 * R1 : end address of region
 * R2 : flags
 *--------------------------------------------------------------------------*/
ENTRY(v7_coherent_cache_range)
	dcache_line_size r2, r3
	sub	r3, r2, #1
	bic	r0, r0, r3
1:	mcr	p15, 0, r0, c7, c11, 1		@ clean D line to the point of unification
	dsb
	mcr	p15, 0, r0, c7, c5, 1		@ invalidate I line
	add	r0, r0, r2
	cmp	r0, r1
	blo	1b
	mov	r0, #0
	mcr	p15, 0, r0, c7, c1, 6		@ invalidate BTB
	dsb
	isb
	mov	pc, lr

DECLARE_CPU_OP(cpu_coherent_range, v7_coherent_cache_range)



ENTRY(v7_dma_flush_all)
	stmfd	sp!, {r4-r5, r7, r9-r11, lr}
	v7_way_op c14
	ldmfd	sp!, {r4-r5, r7, r9-r11, lr}	
	mov	pc, lr

DECLARE_CPU_OP(cpu_dma_flush_all, v7_dma_flush_all)

/*
 *	v7_dma_clean_all()
 *
 *	cleans the entire L1 data cache
 */
ENTRY(v7_dma_clean_all)
	stmfd	sp!, {r4-r5, r7, r9-r11, lr}	
	v7_way_op c10
	ldmfd	sp!, {r4-r5, r7, r9-r11, lr}	
	mov	pc, lr

DECLARE_CPU_OP(cpu_dma_clean_all, v7_dma_clean_all)

