/******************************************************************************
 * event.h
 *
 * A nice interface for passing asynchronous events to guest OSes.
 * (architecture-dependent part)
 *
 */

#ifndef __ARM_EVENT_H__
#define __ARM_EVENT_H__

static inline int guest_events_need_delivery(struct vcpu *v)
{
	return ((v->vcpu_info->evtchn_upcall_pending) &&
	       !(v->vcpu_info->arch.cpsr & VPSR_I_BIT));
}

static inline int guest_event_delivery_is_enabled(struct vcpu *v)
{
	return !(v->vcpu_info->arch.cpsr & VPSR_I_BIT);
}

static inline void guest_event_delivery_disable(struct vcpu *v)
{
	v->vcpu_info->arch.cpsr |= VPSR_I_BIT;
}

static inline void guest_event_delivery_enable(struct vcpu *v)
{
	v->vcpu_info->arch.cpsr &= ~(VPSR_I_BIT);
}

static inline void evtchn_notify(struct vcpu *v)
{
	/*
	 * NB1. 'vcpu_flags' and 'processor' must be checked /after/ update of
	 * pending flag. These values may fluctuate (after all, we hold no
	 * locks) but the key insight is that each change will cause
	 * evtchn_upcall_pending to be polled.
	 *
	 * NB2. We save VCPUF_running across the unblock to avoid a needless
	 * IPI for domains that we IPI'd to unblock.
	 */
	int running = test_bit(_VCPUF_running, &v->vcpu_flags);
	vcpu_unblock(v);
	if ( running )
		smp_send_event_check_cpu(v->processor);
}

#endif /* __ARM_EVENT_H__ */
