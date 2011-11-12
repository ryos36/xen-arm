/*
 * platform.h
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

#ifndef __ARM_PLATFORM_H__
#define __ARM_PLATFORM_H__

#include <asm/memory.h>

#ifdef __ASSEMBLY__
#define DECLARE_PLATFORM_OP(gop, nop)	\
        .set gop, nop                   ;\
	.global gop                     ;
#else
#define DECLARE_PLATFORM_OP(gop, nop)	\
        typeof (nop) gop                \
	__attribute__((weak, alias(#nop)))


struct platform {
	char *name;
	struct meminfo *memory;
};

/*
 * Prototype of platform operations
 */
struct platform *platform_setup(void *param);
void platform_halt(int mode);
#endif
#endif /* __ARM_PLATFORM_H__ */

