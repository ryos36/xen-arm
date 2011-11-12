#ifndef __EVTCHN_H__
#define __EVTCHN_H__

#include <os/os.h>
#include <os/traps.h>
#include <os/hypervisor.h>

#define NR_EVS 1024

/* ev handler status */
#define EVS_INPROGRESS		1	/* Event handler active - do not enter! */
#define EVS_DISABLED		2	/* Event disabled - do not enter! */
#define EVS_PENDING		4	/* Event pending - replay on enable */
#define EVS_REPLAY		8	/* Event has been replayed but not acked yet */

#define force_evtchn_callback() ((void)HYPERVISOR_xen_version(0, 0))

/* this represents a event handler. Chaining or sharing is not allowed */
typedef struct _ev_action_t {
	void (*handler)(int, struct pt_regs *);
	unsigned int status;		/* IRQ status */
	u32 count;
} ev_action_t;


void mask_evtchn(u32 port);
void unmask_evtchn(u32 port);
void clear_evtchn(u32 port);

/* prototypes */
int do_event(u32 port, struct pt_regs *regs);
int bind_virq( u32 virq, void (*handler)(int, struct pt_regs *) );
int bind_evtchn( u32 virq, void (*handler)(int, struct pt_regs *) );
void unbind_evtchn( u32 port );
void evtchn_init(void);
void unbind_virq( u32 port );
void do_hypervisor_callback(struct pt_regs *regs);
int bind_interdomain(u32 dom_id, u32 port, void (*handler)(int, struct pt_regs *));
int alloc_unbound_port(u32 domid, void (*handler)(int, struct pt_regs *));

static inline int notify_remote_via_evtchn(int port)
{
    evtchn_op_t op;
    op.cmd = EVTCHNOP_send;
    op.u.send.port = port;
    return HYPERVISOR_event_channel_op(&op);
}


#endif /* _EVENTS_H_ */
