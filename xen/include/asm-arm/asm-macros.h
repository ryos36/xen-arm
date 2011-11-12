/*
 * asm-macros.h
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

#ifndef __ARM_ASM_MACROS_H__
#define __ARM_ASM_MACROS_H__

#include <asm/system.h>
#include <asm/arch/config.h>

#ifdef __ASSEMBLY__
/*
 * Endian independent macros for shifting bytes within registers.
 */
#ifndef __ARMEB__
#define pull            lsr
#define push            lsl
#define get_byte_0      lsl #0
#define get_byte_1      lsr #8
#define get_byte_2      lsr #16
#define get_byte_3      lsr #24
#define put_byte_0      lsl #0
#define put_byte_1      lsl #8
#define put_byte_2      lsl #16
#define put_byte_3      lsl #24
#else
#define pull            lsl
#define push            lsr
#define get_byte_0      lsr #24
#define get_byte_1      lsr #16
#define get_byte_2      lsr #8
#define get_byte_3      lsl #0
#define put_byte_0      lsl #24
#define put_byte_1      lsl #16
#define put_byte_2      lsl #8
#define put_byte_3      lsl #0
#endif

/*
 * Data preload for architectures that support it
 */
#define PLD(code...)	code

/*
 * LOADREGS - ldm with PC in register list (eg, ldmfd sp!, {pc})
 */
#ifdef __STDC__
#define LOADREGS(cond, base, reglist...)\
        ldm##cond       base,reglist
#else
#define LOADREGS(cond, base, reglist...)\
        ldm/**/cond     base,reglist
#endif

/*
 * Build a return instruction for this processor type.
 */
#define RETINSTR(instr, regs...)\
        instr   regs

#define CTXT_R0			0
#define CTXT_R1			4
#define CTXT_R2			8
#define CTXT_R3			12
#define CTXT_R4			16
#define CTXT_R5			20
#define CTXT_R6			24
#define CTXT_R7			28
#define CTXT_R8			32
#define CTXT_R9			36
#define CTXT_R10		40
#define CTXT_R11		44
#define CTXT_R12		48
#define CTXT_USP		52
#define CTXT_ULR		56
#define CTXT_SSP		60
#define CTXT_SLR		64
#define CTXT_PC			68
#define CTXT_SPSR		72
#define CTXT_EXTRA		76
#define CTXT_FRAME_SIZE	80

#ifdef CONFIG_EABI_SUPPORT
#define SPFIX(code...)	code
#else
#define SPFIX(code...)
#endif

.macro	SWITCH_MODE mode, flags
	msr	cpsr_c, #(\mode | \flags)
.endm

.macro	DISABLE_INTERRUPTS
	msr	cpsr_c, #(PSR_I_BIT | PSR_MODE_SVC)
.endm

.macro	ENABLE_INTERRUPTS
	msr	cpsr_c, #PSR_MODE_SVC
.endm

.macro  disable_irq, temp
	msr		cpsr_c, #PSR_I_BIT | PSR_MODE_SVC
.endm

.macro  enable_irq, temp
	msr		cpsr_c, #PSR_MODE_SVC
.endm 

.macro	cci		rd
	mov		\rd, #STACK_SIZE
	sub		\rd, \rd, #1
	bic		\rd, r13, \rd
.endm

/*
 * Save the current IRQ state and disable IRQs.  Note that this macro
 * assumes FIQs are enabled, and that the processor is in SVC mode.
 */
.macro	save_and_disable_irqs, oldcpsr, temp
	mrs	\oldcpsr, cpsr
	mov	\temp, #PSR_I_BIT | PSR_MODE_SVC
	msr	cpsr_c, \temp
.endm

/*
 * Restore interrupt state previously stored in a register.  We don't
 * guarantee that this will preserve the flags.
 */
.macro	restore_irqs, oldcpsr
	msr	cpsr_c, \oldcpsr
.endm

.macro  cpwait, rd
	mrc     p15, 0, \rd, c2, c0, 0          @ arbitrary read of cp15
	mov     \rd, \rd                        @ wait for completion
	sub     pc, pc, #4                      @ flush instruction pipeline
.endm

.macro  cpwait_ret, lr, rd
	mrc     p15, 0, \rd, c2, c0, 0          @ arbitrary read of cp15
	sub     pc, \lr, \rd, LSR #32           @ wait for completion and
.endm

#define USER(x...)				\
9999:	x;					\
	.section .extable,"a";		\
	.align	3;				\
	.long	9999b,9001f;			\
	.previous

#endif
#endif /* __ARM_ASM_MACROS_H__ */
