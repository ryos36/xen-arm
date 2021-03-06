/*
 *  linux/arch/arm/kernel/debug-armv.S
 *
 *  Copyright (C) 1994-1999 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  32-bit debugging code
 */

#include <xen-asm/config.h>  // for  CONFIG_ARCH_IMX21
#include <linux/linkage.h>      // for ENTRY

                .text
//#if defined(CONFIG_ARCH_IMX21)
#if 1

                .macro  addruart,rx
                mrc     p15, 0, \rx, c1, c0
                tst     \rx, #1                 @ MMU enabled?
                moveq   \rx, #0x10000000        @ Physical
                        @ movne   \rx, #0xe4000000        @ Virtual
                movne   \rx, #0xc00000        @ Virtual for arm-xen  by PCJ
                orrne   \rx, \rx, #0xff000000

                orr     \rx, \rx, #0xa000
                orr     \rx, \rx, #0x40
                .endm

                .macro  senduart,rd,rx
                str     \rd, [\rx]      @ TXDATA
                .endm

                .macro  waituart,rd,rx
                .endm

                .macro  busyuart,rd,rx
1002:           ldr    \rd, [\rx, #0x54]
                @ Check TRDY in USR1_1 (phys=0x1000a094)
                and     \rd, \rd, #0x2000
                cmp     \rd, #0x2000
                bne     1002b
                .endm

#else
#error Unknown architecture
#endif


/*
 * Useful debugging routines
 */
ENTRY(printhex8)
		mov	r1, #8
		b	printhex

ENTRY(printhex4)
		mov	r1, #4
		b	printhex

ENTRY(printhex2)
		mov	r1, #2
printhex:	adr	r2, hexbuf
		add	r3, r2, r1
		mov	r1, #0
		strb	r1, [r3]
1:		and	r1, r0, #15
		mov	r0, r0, lsr #4
		cmp	r1, #10
		addlt	r1, r1, #'0'
		addge	r1, r1, #'a' - 10
		strb	r1, [r3, #-1]!
		teq	r3, r2
		bne	1b
		mov	r0, r2
		b	printascii

		.ltorg

ENTRY(printascii)
		addruart r3
		b	2f
1:		waituart r2, r3
		senduart r1, r3
		busyuart r2, r3
		teq	r1, #'\n'
		moveq	r1, #'\r'
		beq	1b
2:		teq	r0, #0
		ldrneb	r1, [r0], #1
		teqne	r1, #0
		bne	1b
		mov	pc, lr

ENTRY(printch)
		addruart r3
		mov	r1, r0
		mov	r0, #0
		b	1b

hexbuf:		.space 16
