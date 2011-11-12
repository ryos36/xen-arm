#ifndef __ASM_SYSTEM_H
#define __ASM_SYSTEM_H

#include <asm/types.h>

/*
 * PSR bits
 */
#define PSR_MODE_USR26		0x00000000
#define PSR_MODE_FIQ26		0x00000001
#define PSR_MODE_IRQ26		0x00000002
#define PSR_MODE_SVC26		0x00000003
#define PSR_MODE_USR		0x00000010
#define PSR_MODE_FIQ		0x00000011
#define PSR_MODE_IRQ		0x00000012
#define PSR_MODE_SVC		0x00000013
#define PSR_MODE_ABT		0x00000017
#define PSR_MODE_UND		0x0000001b
#define PSR_MODE_SYS		0x0000001f
#define PSR_MODE_MASK		0x0000001f
#define PSR_T_BIT		0x00000020
#define PSR_F_BIT		0x00000040
#define PSR_I_BIT		0x00000080
#define PSR_J_BIT		0x01000000
#define PSR_Q_BIT		0x08000000
#define PSR_V_BIT		0x10000000
#define PSR_C_BIT		0x20000000
#define PSR_Z_BIT		0x40000000
#define PSR_N_BIT		0x80000000
#define PCMASK			0

/*
 * Groups of PSR bits
 */
#define PSR_MASK_FLAGS          0xff000000      /* Flags                */
#define PSR_MASK_STATUS         0x00ff0000      /* Status               */
#define PSR_MASK_EXTENSION      0x0000ff00      /* Extension            */
#define PSR_MASK_CONTROL        0x000000ff      /* Control              */

/*
 * CR1 bits (CP#15 CR1)
 */
#define CR_M	(1 << 0)	/* MMU enable				*/
#define CR_A	(1 << 1)	/* Alignment abort enable		*/
#define CR_C	(1 << 2)	/* Dcache enable			*/
#define CR_W	(1 << 3)	/* Write buffer enable			*/
#define CR_P	(1 << 4)	/* 32-bit exception handler		*/
#define CR_D	(1 << 5)	/* 32-bit data address range		*/
#define CR_L	(1 << 6)	/* Implementation defined		*/
#define CR_B	(1 << 7)	/* Big endian				*/
#define CR_S	(1 << 8)	/* System MMU protection		*/
#define CR_R	(1 << 9)	/* ROM MMU protection			*/
#define CR_F	(1 << 10)	/* Implementation defined		*/
#define CR_Z	(1 << 11)	/* Implementation defined		*/
#define CR_I	(1 << 12)	/* Icache enable			*/
#define CR_V	(1 << 13)	/* Vectors relocated to 0xffff0000	*/
#define CR_RR	(1 << 14)	/* Round Robin cache replacement	*/
#define CR_L4	(1 << 15)	/* LDR pc can set T bit			*/
#define CR_DT	(1 << 16)
#define CR_IT	(1 << 18)
#define CR_ST	(1 << 19)
#define CR_FI	(1 << 21)	/* Fast interrupt (lower latency mode)	*/
#define CR_U	(1 << 22)	/* Unaligned access operation		*/
#define CR_XP	(1 << 23)	/* Extended page tables			*/
#define CR_VE	(1 << 24)	/* Vectored interrupts			*/

#define __asmeq(x, y)  ".ifnc " x "," y " ; .err ; .endif\n\t"

#ifndef __ASSEMBLY__

/*
 * dmb : Data Memory Barrier
 * dsb : Data Synchronization Barrier
 * 	-> Drain Write Buffer in earlier of the architecture
 * isb : Instruction Synchronization Barrier
 * 	-> Flush pipeline and brach target buffers.
 */

#if  (CONFIG_CPU_CORE_VERSION >=7)
#define isb() __asm__ __volatile__ ("isb" : : : "memory")
#define dsb() __asm__ __volatile__ ("dsb" : : : "memory")
#define dmb() __asm__ __volatile__ ("dmb" : : : "memory")
#elif defined(CONFIG_CPU_CORE_XSCALE3) || (CONFIG_CPU_CORE_VERSION ==6)
#define dmb()	__asm__ __volatile__("mcr p15, 0, %0, c7, c10, 5" : : "r"(0) : "memory")
#define isb()	__asm__ __volatile__("mcr p15, 0, %0, c7, c5,  4" : : "r"(0) : "memory")
#define dsb()	__asm__ __volatile__("mcr p15, 0, %0, c7, c10, 4" : : "r"(0) : "memory")
#else
#define dmb() 	__asm__ __volatile__("" : : : "memory")
#define isb()	__asm__ __volatile__("" : : : "memory")
#define dsb()	__asm__ __volatile__("mcr p15, 0, %0, c7, c10, 4" : : "r"(0) : "memory")
#endif

#define mb()		dmb()
#define rmb() 		dmb()
#define wmb() 		dmb()
#define barrier()	dmb()

#define cpu_relax()	dmb()

#ifdef CONFIG_SMP
#define smp_rmb()	rmb()
#define smp_wmb()	wmb()
#define smp_mb()	dmb()
#endif

#if CONFIG_CPU_CORE_VERSION >= 6

#define local_irq_save(x)		\
({					\
	__asm__ __volatile__(		\
		"mrs    %0, cpsr \n"	\
		"cpsid  i"		\
		: "=r" (x)		\
		:			\
		: "memory", "cc");	\
})

#define local_irq_enable()  __asm__("cpsie i    @ __sti" : : : "memory", "cc")
#define local_irq_disable() __asm__("cpsid i    @ __cli" : : : "memory", "cc")
#define local_fiq_enable()  __asm__("cpsie f    @ __stf" : : : "memory", "cc")
#define local_fiq_disable() __asm__("cpsid f    @ __clf" : : : "memory", "cc")

#else

/*
 * Save the current interrupt enable state & disable IRQs
 */
#define local_irq_save(x)			\
({						\
	unsigned long temp;			\
	(void) (&temp == &x);			\
	__asm__ __volatile__(			\
	"	mrs	%0, cpsr\n"		\
	"	orr	%1, %0, #128\n"		\
	"	msr	cpsr_c, %1"		\
	: "=r" (x), "=r" (temp)			\
	:					\
	: "memory", "cc");			\
})
	
/*
 * Enable IRQs
 */
#define local_irq_enable()			\
({						\
	unsigned long temp;			\
	__asm__ __volatile__(			\
	"	mrs	%0, cpsr\n"		\
	"	bic	%0, %0, #128\n"		\
	"	msr	cpsr_c, %0"		\
	: "=r" (temp)				\
	:					\
	: "memory", "cc");			\
})

/*
 * Disable IRQs
 */
#define local_irq_disable()			\
({						\
	unsigned long temp;			\
	__asm__ __volatile__(			\
	"	mrs	%0, cpsr\n"		\
	"	orr	%0, %0, #128\n"		\
	"	msr	cpsr_c, %0"		\
	: "=r" (temp)				\
	:					\
	: "memory", "cc");			\
})

/*
 * Enable FIQs
 */
#define local_fiq_enable()			\
({						\
	unsigned long temp;			\
	__asm__ __volatile__(			\
	"	mrs	%0, cpsr\n"		\
	"	bic	%0, %0, #64\n"		\
	"	msr	cpsr_c, %0"		\
	: "=r" (temp)				\
	:					\
	: "memory", "cc");			\
})

/*
 * Disable FIQs
 */
#define local_fiq_disable()			\
	({					\
		unsigned long temp;		\
		__asm__ __volatile__(		\
		"	mrs	%0, cpsr\n"	\
		"	orr	%0, %0, #64\n"	\
		"	msr	cpsr_c, %0"	\
		: "=r" (temp)			\
		:				\
		: "memory", "cc");		\
	})

#endif

/*
 * Save the current interrupt enable state.
 */
#define local_save_flags(x)			\
	({					\
		__asm__ __volatile__(		\
		"mrs	%0, cpsr\n"		\
		: "=r" (x) : : "memory", "cc");	\
	})

/*
 * restore saved IRQ & FIQ state
 */
#define local_irq_restore(x)			\
	({					\
		__asm__ __volatile__(		\
		"msr	cpsr_c, %0\n"		\
		:				\
		: "r" (x)			\
		: "memory", "cc");		\
	})

#define irqs_disabled()				\
	({					\
		unsigned long flags;		\
		local_save_flags(flags);	\
		flags & PSR_I_BIT;		\
	})

#define local_irq_is_enabled()	(!irqs_disabled())

static inline unsigned int get_cr(void)
{
	unsigned int val;
	asm("mrc p15, 0, %0, c1, c0, 0" : "=r"(val) : : "cc");

	return val;
}

static inline void set_cr(unsigned int val)
{
	asm volatile("mcr p15, 0, %0, c1, c0, 0" : : "r"(val) : "cc");

	isb();
}

static inline unsigned long __xchg(unsigned long x, volatile void * ptr, int size)
{
	unsigned long ret;

#if CONFIG_CPU_CORE_VERSION >= 6
	unsigned int tmp;
#endif

	switch (size) {
#if CONFIG_CPU_CORE_VERSION >= 6
        case 1:
		__asm__ __volatile__(
		"1:     ldrexb  %0, [%3]\n"
		"       strexb  %1, %2, [%3]\n"
		"       teq     %1, #0\n"
		"       bne     1b"
		: "=&r" (ret), "=&r" (tmp)
		: "r" (x), "r" (ptr)
		: "memory", "cc");
		break;
	case 4:
		__asm__ __volatile__("@ __xchg4\n"
		"1:     ldrex   %0, [%3]\n"
		"       strex   %1, %2, [%3]\n"
		"       teq     %1, #0\n"
		"       bne     1b"
		: "=&r" (ret), "=&r" (tmp)
		: "r" (x), "r" (ptr)
		: "memory", "cc");
		break;
#else
	case 1:
		__asm__ __volatile__(
		"swpb	%0, %1, [%2]"
		: "=&r" (ret)
		: "r" (x), "r" (ptr)
		: "memory", "cc");
		break;
	case 4:
		__asm__ __volatile__(
		"  swp	%0, %1, [%2]"
		: "=&r" (ret)
		: "r" (x), "r" (ptr)
		: "memory", "cc");
		break;
#endif
	default:
		ret = 0;
		break;
	}

	return ret;
}

#define cmpxchg(ptr, old, new)						\
({ 									\
	__typeof__(*(ptr)) prev; 					\
	unsigned long flags;						\
	local_irq_save(flags);						\
	prev = *((__typeof__(*(ptr)) *)ptr); 				\
	if(prev == old) 						\
		*((__typeof__(*(ptr)) *)ptr) = (__typeof__(*(ptr)))new;	\
	local_irq_restore(flags);					\
	prev; 								\
})

#define xchg(ptr,v)	\
	((__typeof__(*(ptr)))__xchg((unsigned long)(v),(ptr),sizeof(*(ptr))))

#if 0
#define cmpxchg(v, o, n)		\
	((__typeof__(*(v))) __cmpxchg((v),(unsigned long)(o), (unsigned long)(n), sizeof(*(v))))

static inline u32 __cmpxchg(volatile void *ptr, unsigned long old, unsigned long new, int size)
{
	unsigned long prev, flags;

	local_irq_save(flags);
	switch(size) {
		case 1: 
			prev = *(u8 *)ptr;
			if (prev == old)
				*(u8 *)ptr = (u8)new;
			break;
		case 2: 
			prev = *(u16 *)ptr;
			if (prev == old)
				*(u16 *)ptr = (u16)new;
			break;
		case 4: 
			prev = *(u32 *)ptr;
			if (prev == old)
				*(u32 *)ptr = (u32)new;
			break;
		default:
			prev = 0;
	}
	local_irq_restore(flags);

	return prev;
}
#endif
#endif /* __ASSEMBLY__ */
#endif
