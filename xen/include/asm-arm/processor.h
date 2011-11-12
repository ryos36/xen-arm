/*
 *  processor.h
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
#ifndef __ARM_PROCESSOR_H__
#define __ARM_PROCESSOR_H__

#define user_mode(regs)					\
    (((regs)->spsr & 0xf) == 0)

#define thumb_mode(regs)				\
	(((regs)->spsr & PSR_T_BIT))

#define processor_mode(regs)			\
	((regs)->spsr & PSR_MODE_MASK)

#define interrupts_enabled(regs)		\
	(!((regs)->spsr & PSR_I_BIT))

#define fast_interrupts_enabled(regs)	\
	(!((regs)->spsr & PSR_F_BIT))

#define condition_codes(regs)			\
	((regs)->spsr & (PSR_V_BIT | PSR_C_BIT | PSR_Z_BIT | PSR_N_BIT))

#endif /* __ARM_PROCESSOR_H__ */
