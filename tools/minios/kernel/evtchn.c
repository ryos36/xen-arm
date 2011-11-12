/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*-
 ****************************************************************************
 * (C) 2003 - Rolf Neugebauer - Intel Research Cambridge
 * (C) 2005 - Grzegorz Milos - Intel Research Cambridge
 ****************************************************************************
 *
 *        File: events.c
 *      Author: Rolf Neugebauer (neugebar@dcs.gla.ac.uk)
 *     Changes: Grzegorz Milos (gm281@cam.ac.uk)
 *              
 *        Date: Jul 2003, changes Jun 2005
 * 
 * Environment: Xen Minimal OS
 * Description: Deals with events recieved on event channels
 *
 ****************************************************************************
 */

#include <os/os.h>
#include <os/hypervisor.h>
#include <os/evtchn.h>
#include <os/bitops.h>
#include <os/lib.h>

#define active_evtchns(cpu,sh,idx)              \
    ((sh)->evtchn_pending[idx] &                \
     ~(sh)->evtchn_mask[idx])
     
static ev_action_t ev_actions[NR_EVS];
void default_handler(int port, struct pt_regs *regs);

void do_hypervisor_callback(struct pt_regs *regs)
{	
    u32 	       l1, l2;
    unsigned int   l1i, l2i, port;
    int            cpu = 0;
    shared_info_t *s = HYPERVISOR_shared_info;
    vcpu_info_t   *vcpu_info = &(s->vcpu_info[cpu]);

    /*
    printk("address(HYPERVISOR_shared_info) = 0x%lx, address(vcpu_info) = 0x%lx\n",
    	(unsigned long) HYPERVISOR_shared_info, (unsigned long) vcpu_info);
    printk("evtchn_upcall_pending : %d\n", vcpu_info->evtchn_upcall_pending);
    printk("evtchn_upcall_mask : %d\n", vcpu_info->evtchn_upcall_mask);
    */

	vcpu_info->evtchn_upcall_pending = 0;
 
    /* NB. No need for a barrier here -- XCHG is a barrier on x86. */
    
    l1 = xchg(&vcpu_info->evtchn_pending_sel, 0);

    while ( l1 != 0 )
    {
        l1i = __ffs(l1);
        l1 &= ~(1 << l1i);
        
        while ( (l2 = active_evtchns(cpu, s, l1i)) != 0 )
        {
            l2i = __ffs(l2);
            l2 &= ~(1 << l2i);

            port = (l1i << 5) + l2i;
			do_event(port, regs);
        }
    }
}

void mask_evtchn(u32 port)
{
    shared_info_t *s = HYPERVISOR_shared_info;
    synch_set_bit(port, &s->evtchn_mask[0]);
}

void unmask_evtchn(u32 port)
{
    shared_info_t *s = HYPERVISOR_shared_info;
    vcpu_info_t *vcpu_info = &s->vcpu_info[smp_processor_id()];

    synch_clear_bit(port, &s->evtchn_mask[0]);

    /*
     * The following is basically the equivalent of 'hw_resend_irq'. Just like
     * a real IO-APIC we 'lose the interrupt edge' if the channel is masked.
     */
    if (  synch_test_bit        (port,    &s->evtchn_pending[0]) && 
         !synch_test_and_set_bit(port>>5, &vcpu_info->evtchn_pending_sel) )
    {
        vcpu_info->evtchn_upcall_pending = 1;
        if ( !vcpu_info->evtchn_upcall_mask )
            force_evtchn_callback();
    }
}

void clear_evtchn(u32 port)
{
    shared_info_t *s = HYPERVISOR_shared_info;
    
	clear_bit(port, &s->evtchn_pending[0]);
}


/*
 * Demux events to different handlers.
 */
int do_event(u32 port, struct pt_regs *regs)
{
	ev_action_t  *action;

	if (port >= NR_EVS) {
		printk("Port number too large: %d\n", port);

		return 0;
	}

	action = &ev_actions[port];
	action->count++;

	if (!action->handler)
		goto out;
    
	if (action->status & EVS_DISABLED)
		goto out;
    
	/* call the handler */
	action->handler(port, regs);

	clear_evtchn(port);
    
 out:
	return 1;

}

int bind_evtchn( u32 port, void (*handler)(int, struct pt_regs *) )
{
	
	//if(ev_actions[port].handler)
	//	printk("WARN: Handler for port %d already registered, replacing\n", port);

	ev_actions[port].handler = handler;
	ev_actions[port].status &= ~EVS_DISABLED;	  
 
	/* Finally unmask the port */
	unmask_evtchn(port);

	return port;
}

void unbind_evtchn( u32 port )
{
	//if (!ev_actions[port].handler)
	//	printk("WARN: No handler for port %d when unbinding\n", port);

	ev_actions[port].handler = NULL;
	ev_actions[port].status |= EVS_DISABLED;
}

int bind_virq( u32 virq, void (*handler)(int, struct pt_regs *) )
{
	evtchn_op_t op;
	int ret = 0;
	
	/* Try to bind the virq to a port */
	op.cmd = EVTCHNOP_bind_virq;
	op.u.bind_virq.virq = virq;
	op.u.bind_virq.vcpu = smp_processor_id();

	if ( HYPERVISOR_event_channel_op(&op) != 0 ) {
		ret = 1;
		//printk("Failed to bind virtual IRQ %d\n", virq);
		goto out;
	}
		
	bind_evtchn(op.u.bind_virq.port, handler);

out:
	return ret;
}

int bind_interdomain(u32 dom_id, u32 port, void (*handler)(int, struct pt_regs *))
{
	evtchn_op_t	op;
	int ret = 0;
	
	op.cmd = EVTCHNOP_bind_interdomain;
	op.u.bind_interdomain.remote_dom = dom_id;
	op.u.bind_interdomain.remote_port = port;
	
	if(HYPERVISOR_event_channel_op(&op) != 0) {
		ret = 1;
		//printk("Failed to bind interdomain\n");
		goto out;
	}
	
	bind_evtchn(op.u.bind_interdomain.local_port, handler);
	
	ret = op.u.bind_interdomain.local_port;
	
out:
	return ret;
}

int alloc_unbound_port(u32 domid, void (*handler)(int, struct pt_regs *))
{
	evtchn_op_t	op;
	int ret = 0;

	op.cmd = EVTCHNOP_alloc_unbound;
	op.u.alloc_unbound.dom        = DOMID_SELF;
	op.u.alloc_unbound.remote_dom = domid;
	if(HYPERVISOR_event_channel_op(&op) != 0) {
		ret = 1;
		//printk("Failed to allocate unbound port\n");
		goto out;
	}

	bind_evtchn(op.u.alloc_unbound.port, handler);
	
	ret = op.u.alloc_unbound.port;

out:
	return ret;
}


void unbind_virq( u32 port )
{
	unbind_evtchn(port);
}

/*
 * Initially all events are without a handler and disabled
 */
void evtchn_init(void)
{
    int i;

    /* inintialise event handler */
    for ( i = 0; i < NR_EVS; i++ ) {
        ev_actions[i].status  = EVS_DISABLED;
        ev_actions[i].handler = default_handler;
    }
}

void default_handler(int port, struct pt_regs *regs) {
    printk("[Port %d] - event received\n", port);
}
