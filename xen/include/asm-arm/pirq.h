#ifndef __ARM_PIRQ_H__
#define __ARM_PIRQ_H__

#include <xen/list.h>
#include <xen/cache.h>
#include <xen/spinlock.h>
#include <public/xen.h>
#include <public/event_channel.h>
#include <asm/arch/irqs.h>

#define IRQ_MAX_GUESTS				7

#define IRQF_TRIGGER_NONE			PIRQF_TRIGGER_NONE
#define IRQF_TRIGGER_RISING_EDGE	PIRQF_TRIGGER_RISING_EDGE
#define IRQF_TRIGGER_FALLING_EDGE	PIRQF_TRIGGER_FALLING_EDGE
#define IRQF_TRIGGER_BOTH_EDGE		PIRQF_TRIGGER_BOTH_EDGE
#define IRQF_TRIGGER_HIGH_LEVEL		PIRQF_TRIGGER_HIGH_LEVEL
#define IRQF_TRIGGER_LOW_LEVEL		PIRQF_TRIGGER_LOW_LEVEL
#define IRQF_TRIGGER_PROBE			PIRQF_TRIGGER_PROBE
#define IRQF_TRIGGER_NO_AUTO_ENABLE	PIRQF_TRIGGER_NO_AUTO_ENABLE

#define IRQF_TRIGGER_MASK			(IRQF_TRIGGER_HIGH_LEVEL	| \
									 IRQF_TRIGGER_LOW_LEVEL 	| \
									 IRQF_TRIGGER_RISING_EDGE	| \
									 IRQF_TRIGGER_FALLING_EDGE)

#define IRQF_VALID			0x00000010
#define IRQF_DISABLED		0x00000020
#define IRQF_SHARABLE		0x00000040
#define IRQF_GUEST_BOUND	0x00000080

#define IRQ_IN_PROGRESS		0x00000100			
#define IRQ_DISABLED		0x00000200
#define IRQ_PENDING			0x00000400
#define IRQ_REPLAY			0x00000800
#define IRQ_AUTO_DETECT		0x00001000
#define IRQ_WAITING			0x00002000
#define IRQ_LEVEL			0x00004000
#define IRQ_MASKED			0x00008000
#define IRQ_PER_CPU			0x00010000
#define IRQ_NO_PROBE		0x00020000
#define IRQ_NO_REQUEST		0x00040000
#define IRQ_NO_AUTO_ENABLE	0x00080000
#define IRQ_WAKEUP			0x00100000
#define IRQ_MOVE_PENDING	0x00200000
#define IRQ_NO_BALANCING	0x00400000
#define IRQ_GUEST_BOUND		0x00800000

#define IRQ_NONE			0
#define IRQ_HANDLED			1

struct irqaction;
struct pt_regs;
struct irqdesc;
struct cpu_user_regs;

typedef int irqreturn_t;
typedef void (*irq_control_t)(unsigned int);
typedef void (*irq_handler_t)(unsigned int, struct irqdesc *, struct cpu_user_regs *);

typedef struct irqaction {
	irqreturn_t 		(*handler)(int, void *, struct cpu_user_regs *);
	unsigned long 		flags;
	const char			*name;
	void 				*dev_id;
	struct irqaction 	*next;
	int irq;
}irqaction_t __cacheline_aligned;

typedef struct irq_guest_action {
	u8 		nr_guests;
	u8 		in_flight;
	u8 		shareable;
	unsigned int	ack_type;
	struct domain 	*guest[IRQ_MAX_GUESTS];
} irq_guest_action_t __cacheline_aligned;

typedef struct irqchip {
	char	*trigger_type;
	/*
	 * Acknowledge the IRQ.
	 * If this is a level-based IRQ, then it is expected to mask the IRQ
	 * as well.
	 */
	void (*ack)(unsigned int);
	/*
	 * Mask the IRQ in hardware.
	 */
	void (*mask)(unsigned int);
	/*
	 * Unmask the IRQ in hardware.
	 */
	void (*unmask)(unsigned int);
	/*
	 * Ask the hardware to re-trigger the IRQ.
	 * Note: This method _must_ _not_ call the interrupt handler.
	 * If you are unable to retrigger the interrupt, do not
	 * provide a function, or if you do, return non-zero.
	 */
	int (*retrigger)(unsigned int);
	/*
	 * Set the type of the IRQ.
	 */
	int (*set_type)(unsigned int, unsigned int);
	/*
	 * Set wakeup-enable on the selected IRQ
	 */
	int (*wake)(unsigned int, unsigned int);
}irqchip_t __cacheline_aligned;

typedef struct irqdesc {
	irq_handler_t		handle;
	struct irqchip		*chip;
	struct irqaction 	*action;
	unsigned int		flags;
	unsigned int		status;
	spinlock_t			lock;
	unsigned int		disable_depth;
	/* Is this one of HID interrupts? */
	unsigned int		isHIDirq;
}irqdesc_t __cacheline_aligned;

extern struct irqdesc irq_desc[NR_IRQS];

extern int setup_irq(unsigned int irq, struct irqaction * new);
void set_irq_chip(unsigned int irq, struct irqchip *);
void set_irq_flags(unsigned int irq, unsigned int flags);
void set_irq_handler(unsigned int irq, irq_handler_t handler);
void set_irq_chained_handler(unsigned int irq, irq_handler_t handler);

void disable_irq(unsigned int);
void enable_irq(unsigned int);
int set_irq_type(unsigned int irq, unsigned int type);
int setup_irq(unsigned int, struct irqaction *);

/* IRQ action dispatcher */
void level_irq_handler(unsigned int irq, struct irqdesc *desc, struct cpu_user_regs *regs);
void edge_irq_handler(unsigned int irq, struct irqdesc *desc, struct cpu_user_regs *regs);

#define get_irq_descriptor(irq) (irq_desc + irq)
#endif
