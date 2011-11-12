#ifndef __XEN_IRQ_H__
#define __XEN_IRQ_H__

#include <xen/config.h>
#include <xen/cpumask.h>
#include <xen/spinlock.h>
#include <asm/hardirq.h>

#ifndef ARCH_HAS_IRQ_CONTROL

struct irqaction
{
	void (*handler)(int, void *, struct cpu_user_regs *);
	const char *name;
	void *dev_id;
};

/*
 * IRQ line status.
 */
#define IRQ_INPROGRESS		0x00000100	/* IRQ handler active - do not enter! */
#define IRQ_DISABLED		0x00000200	/* IRQ disabled - do not enter! */
#define IRQ_PENDING		0x00000400	/* IRQ pending - replay on enable */
#define IRQ_REPLAY		0x00000800	/* IRQ has been replayed but not acked yet */
#define IRQ_AUTODETECT		0x00001000	/* IRQ is being autodetected */
#define IRQ_WAITING		0x00002000	/* IRQ not yet seen - for autodetection */
#define IRQ_LEVEL		0x00004000	/* IRQ level triggered */
#define IRQ_MASKED		0x00008000	/* IRQ masked - shouldn't be seen again */
#define IRQ_PER_CPU		0x00010000	/* IRQ is per CPU */
#define IRQ_NOPROBE		0x00020000	/* IRQ is not valid for probing */
#define IRQ_NOREQUEST		0x00040000	/* IRQ cannot be requested */
#define IRQ_NOAUTOEN		0x00080000	/* IRQ will not be enabled on request irq */
#define IRQ_WAKEUP		0x00100000	/* IRQ triggers system wakeup */
#define IRQ_MOVE_PENDING	0x00200000	/* need to re-target IRQ destination */
#define IRQ_NO_BALANCING	0x00400000	/* IRQ is excluded from balancing */

/*
 * Interrupt controller descriptor. This is all we need
 * to describe about the low-level hardware.
 */
struct hw_interrupt_type {
	const char *typename;
	unsigned int (*startup)(unsigned int irq);
	void (*shutdown)(unsigned int irq);
	void (*enable)(unsigned int irq);
	void (*disable)(unsigned int irq);
	void (*ack)(unsigned int irq);
	void (*end)(unsigned int irq);
	void (*set_affinity)(unsigned int irq, cpumask_t mask);
};

typedef struct hw_interrupt_type hw_irq_controller;

#include <asm/irq.h>

/*
 * This is the "IRQ descriptor", which contains various information
 * about the irq, including what kind of hardware handling it has,
 * whether it is disabled etc etc.
 * 
 * Pad this out to 32 bytes for cache and indexing reasons.
 */
typedef struct {
	unsigned int status;                /* IRQ status */
	hw_irq_controller *handler;
	struct irqaction *action;   /* IRQ action list */
	unsigned int depth;         /* nested irq disables */
	spinlock_t lock;
} __cacheline_aligned irq_desc_t;

extern irq_desc_t irq_desc[NR_IRQS];

extern int setup_irq(unsigned int, struct irqaction *);
extern void free_irq(unsigned int);

extern hw_irq_controller no_irq_type;
extern void no_action(int cpl, void *dev_id, struct cpu_user_regs *regs);
#endif

struct domain;
struct vcpu;
extern int pirq_guest_unmask(struct domain *p);
extern int pirq_guest_bind(struct vcpu *p, int irq, int will_share,int noautoen);
extern int pirq_guest_unbind(struct domain *p, int irq);

#endif /* __XEN_IRQ_H__ */
