#include <xen/linkage.h>
#include <asm/asm-macros.h>

                .text

ENTRY(_test_and_set_bit_be)
		eor	r0, r0, #0x18		@ big endian byte ordering
ENTRY(_test_and_set_bit_le)
		add	r1, r1, r0, lsr #3	@ Get byte offset
		and	r3, r0, #7		@ Get bit offset
		mov	r0, #1
		save_and_disable_irqs ip, r2
		ldrb	r2, [r1]
		tst	r2, r0, lsl r3
		orreq	r2, r2, r0, lsl r3
		streqb	r2, [r1]
		restore_irqs ip
		moveq	r0, #0
		RETINSTR(mov,pc,lr)


