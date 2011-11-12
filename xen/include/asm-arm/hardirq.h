#ifndef __ARM_HARDIRQ_H__
#define __ARM_HARDIRQ_H__

#include <xen/config.h>
#include <xen/cache.h>

typedef struct irq_cpu_stat {
	unsigned long __softirq_pending;
	unsigned long __local_irq_count;
	unsigned long __nmi_count;
} __cacheline_aligned irq_cpustat_t;

#include <xen/irq_cpustat.h>    /* Standard mappings for irq_cpustat_t above */

#define in_irq() 	(local_irq_count(smp_processor_id()) != 0)

#define irq_enter()     (local_irq_count(smp_processor_id())++)
#define irq_exit()      (local_irq_count(smp_processor_id())--)

#endif /* __ARM_HARDIRQ_H__ */
