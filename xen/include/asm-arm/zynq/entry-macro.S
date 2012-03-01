/*
 * include/asm-arm/arch-imx/entry-macro.S
 *
 * Low-level IRQ helper macros for iMX-based platforms
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <asm/regs.h>
#include <asm/gic.h>
	.macro	disable_fiq
	.endm
	

	@ on return:
	@ Z=1 no irq pending
	@ Z=0 irq is pending, irqnr is valid
	.macro	get_irqnr_and_base, irqnr, irqstat, base, tmp
	ldr     \base, =gic_cpu_base_addr
	ldr     \base, [\base]
	ldr	\irqstat, [\base, #ICCIAR]
	ldr	\tmp, =1021
	
	bic	\irqnr, \irqstat, #0x1c00
	
	/*
	 *  Interrupts 0-15 are IPI
	 *  16-28 are reserved.
	 *  29-31 are local. 29 is local timer and 30 is local watchdog.
	 *  32-1020 are the interrupts from the rest od the hardware blocks in
	 *  the system.
	 */
	
	@ Z=1 if the IRQ number == 31
	cmp	\irqnr, #29
	@ Z|=1 if IRQ number < 31
	cmpcc	\irqnr, \irqnr
	@ Z|=1 if IRQ number == 1021
	cmpne	\irqnr, \tmp
	@ Z|=1 if IRQ number > 1021
	cmpcs	\irqnr, \irqnr
	1001:
	.endm
	
	
	.macro test_for_ipi, irqnr, irqstat, base, tmp
	bic     \irqnr, \irqstat, #0x1c00
	cmp     \irqnr, #16
	strcc   \irqstat, [\base, #ICCEOIR]
	cmpcs   \irqnr, \irqnr
	.endm
	
	.macro test_for_ltirq, irqnr, irqstat, base, tmp
	bic     \irqnr, \irqstat, #0x1c00
	mov     \tmp, #0
	cmp     \irqnr, #29
	moveq   \tmp, #1
	streq   \irqstat, [\base, #ICCEOIR]
	cmp     \tmp, #0
	.endm
	
