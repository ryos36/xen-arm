/*
 * cpu-ops.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *          JaeMin Ryu  <jm77.ryu@samsung.com>
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

#ifndef __ASM_CPU_OPS_H__
#define __ASM_CPU_OPS_H__

#ifndef __ASSEMBLY__
#define DECLARE_CPU_OP(gop, nop)	\
	typeof (nop) gop		\
	__attribute__((weak, alias(#nop)))


void cpu_halt(int mode);
void cpu_idle(void);

/*
 * MMU Operations
 */
void cpu_switch_ttb(unsigned long, int);

/*
 * Cache operations
 */

void cpu_flush_cache_all(unsigned int flags);
void cpu_flush_cache_range(unsigned long start, unsigned long end, unsigned int flags);
//void cpu_flush_cache_entry(unsigned long addr, unsigned long flags);
void cpu_clean_cache_range(unsigned long start, unsigned long end, unsigned int flags);

/*
 *
 */
void cpu_invalidate_dma_range(unsigned long start, unsigned long end, unsigned int flags);
void cpu_clean_dma_range(unsigned long start, unsigned long end, unsigned int flags);
void cpu_flush_dma_range(unsigned long start, unsigned long end, unsigned int flags);
void cpu_coherent_range(unsigned long start, unsigned long end, unsigned int flags);
/*
 * TLB operations
 */
void cpu_flush_tlb_all(unsigned int flags);
void cpu_flush_tlb_entry(unsigned long addr, unsigned int flags);
void cpu_flush_tlb_range(unsigned long start, unsigned long end, unsigned long flags);

/*
 * Page operations
 */

void cpu_copy_page(void *dst, void *src, unsigned long size);
void cpu_clear_page(void *dst, unsigned long size);
#endif

#ifdef __ASSEMBLY__
#define DECLARE_CPU_OP(gop, nop)	 \
	.set gop, nop			;\
	.global gop			;
#endif

#endif

