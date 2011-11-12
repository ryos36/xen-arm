#ifndef __IRQ_H__
#define __IRQ_H__

#include <os/os.h>
#include <os/evtchn.h>

/* This is a barrier for the compiler only, NOT the processor! */
#define barrier() __asm__ __volatile__("": : :"memory")

#define mb() __asm__ __volatile__ ("" : : : "memory")
#define rmb()   mb()
#define wmb()	mb()

/* 
 * The use of 'barrier' in the following reflects their use as local-lock
 * operations. Reentrancy must be prevented (e.g., __cli()) /before/ following
 * critical operations are executed. All critical operations must complete
 * /before/ reentrancy is permitted (e.g., __sti()). Alpha architecture also
 * includes these barriers, for example.
 */

#define __cli()								\
do {									\
	vcpu_info_t *_vcpu;						\
	_vcpu = &HYPERVISOR_shared_info->vcpu_info[smp_processor_id()];	\
	_vcpu->evtchn_upcall_mask = 1;					\
	barrier();							\
} while (0)

#define __sti()								\
do {									\
	vcpu_info_t *_vcpu;						\
	barrier();							\
	_vcpu = &HYPERVISOR_shared_info->vcpu_info[smp_processor_id()];	\
	_vcpu->evtchn_upcall_mask = 0;					\
	barrier(); /* unmask then check (avoid races) */		\
	if ( unlikely(_vcpu->evtchn_upcall_pending) )			\
		force_evtchn_callback();				\
} while (0)

#define __save_flags(x)							\
do {									\
	vcpu_info_t *_vcpu;						\
	_vcpu = &HYPERVISOR_shared_info->vcpu_info[smp_processor_id()];	\
	(x) = _vcpu->evtchn_upcall_mask;				\
} while (0)

#define __restore_flags(x)						\
do {									\
	vcpu_info_t *_vcpu;						\
	barrier();							\
	_vcpu = &HYPERVISOR_shared_info->vcpu_info[smp_processor_id()];	\
	if ((_vcpu->evtchn_upcall_mask = (x)) == 0) {			\
		barrier(); 											\
		if ( unlikely(_vcpu->evtchn_upcall_pending) )		\
			force_evtchn_callback();			\
	}\
} while (0)


#define safe_halt()		((void)0)

#define __save_and_cli(x)						\
do {									\
	vcpu_info_t *_vcpu;						\
	_vcpu = &HYPERVISOR_shared_info->vcpu_info[smp_processor_id()];	\
	(x) = _vcpu->evtchn_upcall_mask;				\
	_vcpu->evtchn_upcall_mask = 1;					\
	barrier();							\
} while (0)

#define local_irq_save(x)	__save_and_cli(x)
#define local_irq_restore(x)	__restore_flags(x)
#define local_save_flags(x)	__save_flags(x)
#define local_irq_disable()	__cli()
#define local_irq_enable()	__sti()

#define irqs_disabled()			\
    HYPERVISOR_shared_info->vcpu_info[smp_processor_id()].evtchn_upcall_mask

/*
 * Make sure gcc doesn't try to be clever and move things around
 * on us. We need to use _exactly_ the address the user gave us,
 * not some alias that contains the same information.
 */
typedef struct { volatile int counter; } atomic_t;

#endif
