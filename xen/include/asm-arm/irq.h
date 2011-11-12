/*
 * irq.h
 *
 * Copyright (C) 2008 Samsung Electronics
 *          Jaemin Ryu <jm77.ryu@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __ARM_IRQ_H__
#define __ARM_IRQ_H__

#include <xen/config.h>
#include <xen/list.h>
#include <xen/cache.h>
#include <xen/spinlock.h>
#include <xen/cpumask.h>
#include <public/xen.h>
#include <public/event_channel.h>

#include <asm/arch/irqs.h>

#define IRQ_MAX_GUESTS		7

#define IRQF_TRIGGER_NONE	0x00000000
#define IRQF_TRIGGER_RISING	0x00000001
#define IRQF_TRIGGER_FALLING	0x00000002
#define IRQF_TRIGGER_HIGH	0x00000004
#define IRQF_TRIGGER_LOW	0x00000008
#define IRQF_TRIGGER_MASK	(IRQF_TRIGGER_HIGH | IRQF_TRIGGER_LOW | \
						 IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)
#define IRQF_TRIGGER_PROBE	0x00000010

#define IRQF_TRIGGER_RISING_EDGE	(0x00000001)
#define IRQF_TRIGGER_FALLING_EDGE	(0x00000002)
#define IRQF_TRIGGER_BOTH_EDGE		(0x00000003)
#define IRQF_TRIGGER_HIGH_LEVEL		(0x00000004)
#define IRQF_TRIGGER_LOW_LEVEL		(0x00000008)
#define IRQF_TRIGGER_NO_AUTO_ENABLE	PIRQF_TRIGGER_NO_AUTO_ENABLE

//#define IRQF_TRIGGER_MASK		(IRQF_TRIGGER_HIGH_LEVEL	| \
					 IRQF_TRIGGER_LOW_LEVEL 	| \
					 IRQF_TRIGGER_RISING_EDGE	| \
					 IRQF_TRIGGER_FALLING_EDGE)

#define IRQF_DISABLED		0x00000020
#define IRQF_SAMPLE_RANDOM	0x00000040
#define IRQF_SHARED		0x00000080
#define IRQF_PROBE_SHARED	0x00000100
#define IRQF_TIMER		0x00000200
#define IRQF_PERCPU		0x00000400
#define IRQF_NOBALANCING	0x00000800
#define IRQF_IRQPOLL		0x00001000

#define IRQF_VALID			0x00000010
#define IRQF_SHARABLE			0x00000040
#define IRQF_GUEST_BOUND		0x00000080

#define IRQ_IN_PROGRESS			0x00000100			
#define IRQ_DISABLED			0x00000200
#define IRQ_PENDING			0x00000400
#define IRQ_REPLAY			0x00000800
#define IRQ_AUTO_DETECT			0x00001000
#define IRQ_WAITING			0x00002000
#define IRQ_LEVEL			0x00004000
#define IRQ_MASKED			0x00008000
#define IRQ_PER_CPU			0x00010000
#define IRQ_NO_PROBE			0x00020000
#define IRQ_NO_REQUEST			0x00040000
#define IRQ_NO_AUTO_ENABLE		0x00080000
#define IRQ_WAKEUP			0x00100000
#define IRQ_MOVE_PENDING		0x00200000
#define IRQ_NO_BALANCING		0x00400000
#define IRQ_GUEST_BOUND			0x00800000

#define IRQ_NONE			0
#define IRQ_HANDLED			1

struct irqaction;
struct pt_regs;
struct irqdesc;
struct seq_file;
struct cpu_user_regs;


typedef int irqreturn_t;
typedef void (*irq_control_t)(unsigned int);
typedef void (*irq_handler_t)(unsigned int, struct irqdesc *, struct cpu_user_regs *);

typedef struct irqaction {
	irqreturn_t 		(*handler)(int, void *, struct cpu_user_regs *);
	unsigned long 		flags;
	const char 		*name;
	void 			*dev_id;
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

        void (*set_affinity)(unsigned int irq, cpumask_t dest);
}irqchip_t __cacheline_aligned;

typedef struct irqdesc {
	char			*type;
	irq_handler_t		handle;
	struct irqchip		*chip;
	struct irqaction 	*action;
	unsigned int		flags;
	unsigned int		status;
	spinlock_t		lock;
	void			*chipdata;
	void			*data;
	unsigned int		disable_depth;
	/* Is this one of HID interrupts? */
	unsigned int		isHIDirq;
	unsigned int		cpu;
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
int pirq_guest_eoi(struct domain *d, unsigned int irq);
#define get_irq_descriptor(irq) (irq_desc + irq)

#define set_irq_data(irq,d)                     do { irq_desc[irq].data = d; } while (0)
#define set_irq_chipdata(irq,d)                 do { irq_desc[irq].chipdata = d; } while (0)

#define get_irq_data(irq)                       (irq_desc[irq].data)
#define get_irq_chip(irq)                       (irq_desc[irq].chip)
#define get_irq_chip_data(irq)                  (irq_desc[irq].chipdata)

#endif
