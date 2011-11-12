/*
 * linkage.h
 *
 * Copyright (C) 2008 Samsung Electronics
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

#ifndef __ASM_ARM_LINKAGE_H__
#define __ASM_ARM_LINKAGE_H__

#ifdef __ASSEMBLY__
#define __ALIGN		.align 0
#define __ALIGN_STR	".align 0"
#endif

#define HYPERCALL_HANDLER	__attribute__((interrupt("SWI")))
#define INTERRUPT_HANDLER	__attribute__((interrupt("IRQ")))
#define NAKED			__attribute__((naked))
#endif
