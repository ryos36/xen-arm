#ifndef __ARM_SMP_H__
#define __ARM_SMP_H__

#include <xen/linkage.h>
#include <xen/spinlock.h>
#include <xen/cpumask.h>
#include <xen/percpu.h>
#include <asm/page.h>
#include <asm/current.h>
#include <asm/arch/smp.h>

/* State of each CPU */
#define CPU_ONLINE	0x0002
#define CPU_DEAD	0x0004

DECLARE_PER_CPU(int, cpu_state);

#if 0
#define raw_smp_processor_id()		 (current_cpu_info()->cpu)
#else
#define raw_smp_processor_id()		(hard_smp_processor_id())
#endif
#define _raw_spin_lock_recursive(_lock)            \
    do {                                           \
        int cpu = smp_processor_id();              \
        if ( likely((_lock)->recurse_cpu != cpu) ) \
        {                                          \
            spin_lock(_lock);                      \
            (_lock)->recurse_cpu = cpu;            \
        }                                          \
        (_lock)->recurse_cnt++;                    \
    } while ( 0 )

#define _raw_spin_unlock_recursive(_lock)          \
    do {                                           \
        if ( likely(--(_lock)->recurse_cnt == 0) ) \
        {                                          \
            (_lock)->recurse_cpu = -1;             \
            spin_unlock(_lock);                    \
        }                                          \
    } while ( 0 )


#endif /* __ARM_SMP_H__ */
