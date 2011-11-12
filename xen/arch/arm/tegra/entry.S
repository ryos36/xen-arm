/*
 * entry.S
 *
 * Copyright (C) 2008 Samsung Electronics
 *         Sang-bum Suh <sbuk.suh@samsung.com>
 *         JaeMin Ryu   <jm77.ryu@samsung.com>
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
#inlcude <asm/arch/irqs.h>
#include <asm/page.h>
#include <asm/system.h>
#include <asm/asm-macros.h>
#include <asm/cpu-domain.h>
#include <asm/asm-offsets.h>

	.align	5

ENTRY(arch_context_switch)
#if 0
	mrc	p15, 0, r4, c13, c0, 3		@ read TLS register

	str	r4, [r1, #(OFFSET_EXT_REGS + OFFSET_WCON)] @temp

	ldr	r4, [r2, #(OFFSET_EXT_REGS + OFFSET_WCON)]
	
	mcr p15, 0, r4, c13, c0, 3
#endif
	mov	pc, lr

