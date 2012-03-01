/*
 *
 * Timer and clock source support for ZYNQ
 *
 * Copyright (C) 2012 Sinby Corp.
 *         Ryos Suzuki ryos at sinby.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <xen/sched.h>
#include <xen/irq.h>
#include <xen/init.h>
#include <xen/softirq.h>
#include <xen/spinlock.h>
#include <asm/time.h>
#include <asm/irq.h>
#include <asm/arch/irqs.h>
#include <asm/arch/zynq.h>

#include "ttc_timer.h"

struct zynq_clocksource_private {
	unsigned long io_base; 
	struct irqaction *action;
};

static irqreturn_t
zynq_clocksource_interrupt(int irq, void *dev_id, struct cpu_user_regs *regs)
{
	struct zynq_clocksource_private *zc_priv;
	uint32_t value;

	zc_priv = (struct zynq_clocksource_private *)dev_id;

        /* Acknowledge the interrupt */
	value = mmio_readl(zc_priv->io_base + XTTCPSS_ISR_OFFSET);
	mmio_writel(value, zc_priv->io_base + XTTCPSS_ISR_OFFSET);

        timer_tick(100);

#if defined(CONFIG_SMP) && !defined(CONFIG_LOCAL_TIMERS)
	smp_send_timer();
#endif
	return IRQ_HANDLED;
}

static struct zynq_clocksource_private zynq_clocksource_private;
static struct irqaction zynq_clocksource_action = {
	.name           = "clock source",
	.flags          = IRQF_DISABLED | IRQF_TIMER,
	.handler        = zynq_clocksource_interrupt,
	.dev_id		= &zynq_clocksource_private,
};

static
void ttc_set_interval(struct zynq_clocksource_private *zc_priv, uint32_t cycles)
{
	uint32_t ctrl_reg;

	/* Disable the counter, set the counter value  and re-enable counter */
	ctrl_reg = mmio_readl(zc_priv->io_base + XTTCPSS_CNT_CNTRL_OFFSET);
	ctrl_reg |= ~(XTTCPSS_CNT_CNTRL_ENABLE_MASK);
	mmio_writel(ctrl_reg, zc_priv->io_base + XTTCPSS_CNT_CNTRL_OFFSET);

#ifdef DEBUG
	printk("set_interval, name = %s, period = %08X\n", zc_priv->action->name, cycles);
#endif
	mmio_writel(cycles, zc_priv->io_base + XTTCPSS_INTR_VAL_OFFSET);

	/* Reset the counter (0x10) so that it starts from 0, one-shot
	   mode makes this needed for timing to be right. */
	ctrl_reg &= XTTCPSS_CNT_CNTRL_ENABLE_MASK;
	ctrl_reg |= 0x10;
	mmio_writel(ctrl_reg, zc_priv->io_base + XTTCPSS_CNT_CNTRL_OFFSET);
}

void zynq_timer_init(void)
{
	zynq_clocksource_private.io_base = IO_ADDRESS(ZYNQ_PERIPH_TTC0_BASE);
	zynq_clocksource_private.action = &zynq_clocksource_action;

	/* Disable counter, Enable Interval mode, Count up timer,
	 * Disable Match mode, Internal Clock source select, set
	 * prescalar to 32, and Enable the Interval interrupt */

	mmio_writel(0x23, zynq_clocksource_private.io_base + XTTCPSS_CNT_CNTRL_OFFSET);
	mmio_writel(0x9, zynq_clocksource_private.io_base + XTTCPSS_CLK_CNTRL_OFFSET);
	mmio_writel(0x1, zynq_clocksource_private.io_base + XTTCPSS_IER_OFFSET);

	/* set interval */
	ttc_set_interval(&zynq_clocksource_private, ~0);

	if (setup_irq(INT_TTC0_0, &zynq_clocksource_action)) {
		printf("Failed to register clocksource IRQ\n");
		BUG();
	}
}
