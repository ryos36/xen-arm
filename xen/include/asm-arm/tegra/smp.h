#ifndef ASMARM_ARCH_SMP_H
#define ASMARM_ARCH_SMP_H


#include <asm/gic.h>

#define hard_smp_processor_id()			\
	({						\
		unsigned int cpunum;			\
		__asm__("mrc p15, 0, %0, c0, c0, 5"	\
			: "=r" (cpunum));		\
		cpunum &= 0x0F;				\
	})

static inline void send_ipi(cpumask_t callmap, int ipi)
{
	gic_raise_softirq(callmap, ipi);
}

/*
 * Do nothing on MPcore.
 */
static inline void smp_cross_call_done(cpumask_t callmap)
{
}

#endif
