/******************************************************************************
 * arch/x86/irq.c
 *
 * Portions of this file are:
 *  Copyright (C) 1992, 1998 Linus Torvalds, Ingo Molnar
 */

#include <xen/config.h>
#include <xen/lib.h>
#include <asm/irq.h>
#include <xen/types.h>
#include <xen/init.h>
#include <xen/errno.h>
#include <xen/spinlock.h>
#include <public/event_channel.h>
#include <public/physdev.h>
#include <public/arch-arm.h>
#include <xen/bitmap.h>
#include <xen/sched.h>
#include <xen/event.h>
#include <asm/linkage.h>	
#include <asm/hardirq.h> 
#include <security/acm/acm_hooks.h>
#include <xen/config.h>

#define ACK_TYPE_NONE	0
#define ACK_TYPE_UNMASK	1

/* Global interrupt descriptor table */
struct irqdesc irq_desc[NR_IRQS];

unsigned int foreground_domain = 0;

extern void send_guest_pirq(struct domain *, int );

void enable_fiq(int fiq)
{
}

void disable_fiq(int fiq)
{
}

int pirq_guest_eoi(struct domain *d, unsigned int irq)
{
	unsigned int flags;
	struct irqdesc *desc;

	desc = get_irq_descriptor(irq);

	spin_lock_irqsave(&desc->lock, flags);
	if ( test_and_clear_bit(irq, d->pirq_mask) && (--((irq_guest_action_t *)desc->action)->in_flight == 0)) {
		desc->chip->unmask(irq);
	}

	spin_unlock_irqrestore(&desc->lock, flags);

	return 0;
}

asmlinkage int do_set_HID_irq(unsigned int irq)
{
	struct irqdesc *desc = irq_desc + irq;
	unsigned long flags;

	spin_lock_irqsave(&desc->lock, flags);
	if(acm_set_HID_irq(irq)) {
		desc->isHIDirq = 1;
	}
	spin_unlock_irqrestore(&desc->lock, flags);

	return 0;
}

asmlinkage int do_set_foreground_domain(unsigned int dom)
{
	if(acm_set_foregrounddom())
		foreground_domain = dom;

	return 0;
}

int do_set_irq_type(unsigned int irq, unsigned int type)
{
	if(acm_set_irq_type())
 	   return set_irq_type(irq, type);
}

static int pirq_ack_type(int irq)
{
	struct irqdesc *desc;
	
	desc = get_irq_descriptor(irq);

        if (!strcmp(desc->chip->trigger_type, "level"))
	        return ACK_TYPE_UNMASK;

	if (!strcmp(desc->chip->trigger_type, "edge"))
		return ACK_TYPE_NONE;

	return ACK_TYPE_NONE;
}

static void handle_guest_bound_irq(unsigned int irq, struct irqdesc *desc)
{
	int i;
	struct domain           *d;
	irq_guest_action_t      *action;
	unsigned long flags;

	action = (irq_guest_action_t *)desc->action;

	for ( i = 0; i < action->nr_guests; i++ ) {
		d = action->guest[i];

		// send HID irqs to only the foreground domain.
		if (desc->isHIDirq && d->domain_id != (domid_t)foreground_domain) {
			continue;
		}
		
		if(acm_send_guest_pirq(d, irq)){
			if(action->ack_type == ACK_TYPE_UNMASK) {
				if(!test_and_set_bit(irq, d->pirq_mask)) {
					action->in_flight++;
				}
			}

			send_guest_pirq(d, irq);
		}
	}
}

#if 0
int pirq_guest_unmask(struct domain *d)
{
	u32		m;
	unsigned int	i, j, pirq;
	struct irqdesc	*desc;
	struct shared_info	*s = d->shared_info;

	for ( i = 0; i < ARRAY_SIZE(d->pirq_mask); i++ ) {
		m = d->pirq_mask[i];
		while ( m != 0 ) {
			j = __ffs(m);
			m &= ~(1 << j);
			pirq = (i << 5) + j;

			desc = get_irq_descriptor(pirq);

			spin_lock_irq(&desc->lock);

			if ( !test_bit(d->pirq_to_evtchn[pirq], &s->evtchn_mask[0]) &&
	      	 	     test_and_clear_bit(pirq, (volatile unsigned long *)&d->pirq_mask) &&
       			     (--((irq_guest_action_t *)desc->action)->in_flight == 0) ) {
	      				desc->chip->unmask(pirq);
				}
	
			spin_unlock_irq(&desc->lock);
		}
	}

	return 0;
}
#endif

int pirq_guest_bind(struct vcpu *v, int irq, int will_share,int noautoen)
{
	struct irqdesc      *desc;
	irq_guest_action_t  *action;
	unsigned long       flags;
	int                 rc = 0;

	if ( (irq < 0) || (irq >= NR_IRQS) )
		return -EINVAL;

	desc = get_irq_descriptor(irq);

	spin_lock_irqsave(&desc->lock, flags);

	action = (irq_guest_action_t *)desc->action;

	if (!(desc->flags & IRQF_GUEST_BOUND)) {
		if (desc->action != NULL ) {
			DPRINTK(3,"Cannot bind IRQ %d to guest. In use by %s.\n",(int)irq, desc->action->name);
			rc = -EBUSY;
			goto out;
		}

		action = xmalloc(irq_guest_action_t);
		if ((desc->action = (struct irqaction *)action) == NULL ) {
			DPRINTK(3,"Cannot bind IRQ %d to guest. Out of memory.\n", irq);
			rc = -ENOMEM;
			goto out;
		}

		action->shareable = 1;
		action->nr_guests = 0;
		action->in_flight = 0;
		action->ack_type  = pirq_ack_type(irq);

		if(noautoen){
			desc->disable_depth = 1;
			action->in_flight++;
			set_bit(irq, v->domain->pirq_mask);
		}
		else
			desc->disable_depth = 0;
		
		desc->flags |= IRQF_GUEST_BOUND;
		if(will_share) {
			desc->flags |= IRQF_SHARABLE;
		}

		if(!noautoen)
			desc->chip->unmask(irq);

	} else if ( !will_share || !action->shareable ) {
		DPRINTK(3,"Cannot bind IRQ %d to guest. Will not share with others.\n", irq);
		rc = -EBUSY;
		goto out;
	}

	if ( action->nr_guests == IRQ_MAX_GUESTS ) {
		DPRINTK(3,"Cannot bind IRQ %d to guest. Already at max share.\n", irq);
		rc = -EBUSY;
		goto out;
	}

	action->guest[action->nr_guests++] = v->domain;

out:
	spin_unlock_irqrestore(&desc->lock, flags);

	return rc;
}

int pirq_guest_unbind(struct domain *d, int irq)
{
	struct irqdesc		*desc;
	irq_guest_action_t 	*action;
	unsigned long		flags;
	int			i;

	desc = get_irq_descriptor(irq);

	spin_lock_irqsave(&desc->lock, flags); 

	action = (irq_guest_action_t *)desc->action;

	if ( action->ack_type == ACK_TYPE_UNMASK ) {
		if ( test_and_clear_bit(irq,d->pirq_mask) && 
		   (--action->in_flight == 0) ) {
			desc->chip->unmask(irq);
		}
	}

	if ( action->nr_guests == 1 ) {
		desc->action = NULL;
		xfree(action);
		desc->disable_depth = 1; 

		desc->flags &= ~IRQF_GUEST_BOUND;
	} else {
		i = 0;
		while ( action->guest[i] && (action->guest[i] != d) )
			i++;
		memmove(&action->guest[i], &action->guest[i+1], IRQ_MAX_GUESTS-i-1);
		action->nr_guests--;
	}

	spin_unlock_irqrestore(&desc->lock, flags);    

	return 0;
}

int setup_irq(unsigned int irq, struct irqaction *new)
{
	int rc = 0;
        int shared = 0;
        struct irqaction *old, **p;
        unsigned long flags;
        struct irqdesc *desc;

	if(irq >= NR_IRQS) {
		printk("BAD IRQ = %d\n", irq);
	}

        desc = get_irq_descriptor(irq);

        spin_lock_irqsave(&desc->lock, flags);

        p = &desc->action;

        if ((old = *p) != NULL) {
                /* Can't share interrupts unless both agree to */
                if (!(old->flags & new->flags & IRQF_SHARABLE) ||
		    ((old->flags ^ new->flags) & IRQF_TRIGGER_MASK)) {
		    	printk("IRQ sharing failed.\n");

			rc = -EBUSY;
			goto out;
                }
                /* add new interrupt at end of irq queue */
                do {
                        p = &old->next;
                        old = *p;
                } while (old);
                shared = 1;
        }

        *p = new;

        if (!shared) {
		if(new->flags & IRQF_TRIGGER_MASK) {
			if(desc->chip->set_type) {
				desc->chip->set_type(irq, new->flags & IRQF_TRIGGER_MASK);
			} else {
				printk("No set_irq_type function for irq %d\n", irq);
				rc = -1;
				goto out;
			}
		}

		desc->status &= ~(IRQ_AUTO_DETECT | IRQ_WAITING | IRQ_IN_PROGRESS);

		if(!(desc->status & IRQ_NO_AUTO_ENABLE)) {
			desc->disable_depth = 0;
			desc->status &= ~IRQ_DISABLED;
			desc->chip->unmask(irq);
		} else {
			desc->disable_depth = 1;
		}

        }

	desc->flags &= ~IRQF_GUEST_BOUND;

out:
        spin_unlock_irqrestore(&desc->lock, flags);

        return rc;
}


int set_irq_chip_data(unsigned int irq, void *data)
{
	struct irqdesc *desc = irq_desc + irq;
	unsigned long flags;

	if (irq >= NR_IRQS || !desc->chip) {
		printk("BUG: bad set_irq_chip_data(IRQ#%d)\n", irq);
		return -EINVAL;
	}

	spin_lock_irqsave(&desc->lock, flags);
	desc->chipdata = data;
	spin_unlock_irqrestore(&desc->lock, flags);

	return 0;
}


void set_irq_chip(unsigned int irq, struct irqchip *chip)
{
	struct irqdesc *desc;
	unsigned long flags;

	if (irq >= NR_IRQS) {
		printk("Trying to install chip for IRQ%d\n", irq);
		return;
	}

	if (chip == NULL) {
		printk("BAD CHIP BUG!!!\n");
		while(1);
	}

	desc = get_irq_descriptor(irq);

	spin_lock_irqsave(&desc->lock, flags);

	desc->chip = chip;

	spin_unlock_irqrestore(&desc->lock, flags);
}

void set_irq_handler(unsigned int irq, irq_handler_t handler)
{
	struct irqdesc *desc;
	unsigned long flags;

	if (irq >= NR_IRQS) {
		printk("Trying to install handler for IRQ%d\n", irq);
		return;
	}

	if (handler == NULL) {
		printk("Handler is not specificed\n");
		while(1);
	}

	desc = get_irq_descriptor(irq);

	spin_lock_irqsave(&desc->lock, flags);

	desc->handle = handler;

	spin_unlock_irqrestore(&desc->lock, flags);
}

void set_irq_chained_handler(unsigned int irq, irq_handler_t handler)
{
	struct irqdesc *desc;
	unsigned long flags;

	if (irq >= NR_IRQS) {
		printk("Trying to install handler for IRQ%d\n", irq);
		return;
	}

	if (handler == NULL) {
		printk("Handler is not specificed\n");
	}

	desc = get_irq_descriptor(irq);

	spin_lock_irqsave(&desc->lock, flags);

	desc->handle = handler;
	desc->chip->unmask(irq);

	spin_unlock_irqrestore(&desc->lock, flags);
}

void set_irq_flags(unsigned int irq, unsigned int iflags)
{
	unsigned long flags;
	struct irqdesc *desc;

	if (irq >= NR_IRQS) {
		printk("Trying to set irq flags for IRQ%d\n", irq);
		while(1);
	}

	desc = get_irq_descriptor(irq);

	spin_lock_irqsave(&desc->lock, flags);

	desc->flags = iflags;

	spin_unlock_irqrestore(&desc->lock, flags);
}

asmlinkage int set_irq_type(unsigned int irq, unsigned int type)
{
	int ret;
	struct irqdesc *desc;
	unsigned long flags;

	if(irq >= NR_IRQS) {
		printk("Trying to set irq type for IRQ%d\n", irq);
		return -1;
	}

	desc = get_irq_descriptor(irq);

	if (type & PIRQT_HID) {
		desc->isHIDirq = 1;
	}
		
	if(desc->chip->set_type) {
		spin_lock_irqsave(&desc->lock, flags);

		ret = desc->chip->set_type(irq, type);

		spin_unlock_irqrestore(&desc->lock, flags);
	}

	return 0;
}

irqreturn_t handle_event(unsigned int irq, struct irqaction *action, struct cpu_user_regs *regs)
{
	u32 status = 0;
	irqreturn_t ret, retval = IRQ_NONE;

	if (!(action->flags & IRQF_DISABLED))
		local_irq_enable();

	do {
		ret = action->handler(irq, action->dev_id, regs);
		if (ret == IRQ_HANDLED)
			status |= action->flags;

		retval |= ret;
		action = action->next;
	} while (action);

	local_irq_disable();

	return retval;
}


void level_irq_handler(unsigned int irq, struct irqdesc *desc, struct cpu_user_regs *regs)
{
	irqreturn_t ret;
	irqaction_t *action;

	spin_lock(&desc->lock);
#if 1
	desc->chip->mask(irq);
#endif
	desc->chip->ack(irq);

	if(desc->flags & IRQF_GUEST_BOUND) {
		handle_guest_bound_irq(irq, desc);
		goto out_unlock;
	}

	if (unlikely(desc->status & IRQ_IN_PROGRESS))
		goto out_unlock;

	desc->status &= ~(IRQ_REPLAY | IRQ_WAITING);

	action = desc->action;
	if (unlikely(!action || (desc->status & IRQ_DISABLED))) {
		desc->status |= IRQ_PENDING;
		goto out_unlock;
	}

	desc->status |= IRQ_IN_PROGRESS;
	desc->status &= ~IRQ_PENDING;

	spin_unlock(&desc->lock);

	ret = handle_event(irq, action, regs);
	if(!ret) {
		printk("Action return = %d\n", ret);
	}

	spin_lock(&desc->lock);
	desc->status &= ~IRQ_IN_PROGRESS;

	if (!(desc->status & IRQ_DISABLED) && desc->chip->unmask)
		desc->chip->unmask(irq);

out_unlock:
	spin_unlock(&desc->lock);
}

void edge_irq_handler(unsigned int irq, struct irqdesc *desc, struct cpu_user_regs *regs)
{
	irqreturn_t ret;
	irqaction_t *action;

	spin_lock(&desc->lock);

	desc->status &= ~(IRQ_REPLAY | IRQ_WAITING);

	if (unlikely((desc->status & (IRQ_IN_PROGRESS | IRQ_DISABLED)) || !desc->action)) {
		desc->status |= (IRQ_PENDING | IRQ_MASKED);
		desc->chip->mask(irq);

		goto out_unlock;
	}

	desc->chip->ack(irq);

	if(desc->flags & IRQF_GUEST_BOUND) {
		handle_guest_bound_irq(irq, desc);
		goto out_unlock;
	}

	/* Mark the IRQ currently in progress.*/
	desc->status |= IRQ_IN_PROGRESS;

	do {
		action = desc->action;

		if (unlikely(!action)) {
			desc->chip->mask(irq);
			goto out_unlock;
		}

		if (unlikely((desc->status &
			(IRQ_PENDING | IRQ_MASKED | IRQ_DISABLED)) ==
			(IRQ_PENDING | IRQ_MASKED))) {
			desc->chip->unmask(irq);
			desc->status &= ~IRQ_MASKED;
		}

		desc->status &= ~IRQ_PENDING;

		spin_unlock(&desc->lock);
		ret = handle_event(irq, action, regs);
		if(!ret) {
			printk("Action return = %d\n", ret);
		}

		spin_lock(&desc->lock);
	} while ((desc->status & (IRQ_PENDING | IRQ_DISABLED)) == IRQ_PENDING);

	desc->status &= ~IRQ_IN_PROGRESS;

out_unlock:
	spin_unlock(&desc->lock);
}

asmlinkage void handle_interrupts(unsigned int irq, struct cpu_user_regs *regs)
{
        struct irqdesc *desc;

	if (irq >= NR_IRQS) {
		printk("Bad IRQ = %d\n", irq);
        }

	desc = get_irq_descriptor(irq);

	desc->handle(irq, desc, regs);

}
