/*
 * arch/arm/mach-tegra/timer.c
 *
 * Timer and clock event support for NVIDIA Tegra SoCs
 *
 * Copyright (c) 2008-2009, NVIDIA Corporation.
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
#include <asm/arch/tegra.h>

#define CLK_RST_CONTROLLER_OSC_CTRL_0 0x50

#define TIMER1_OFFS  0x00  /* reserved for AVP */
#define TIMER2_OFFS  0x08  /* reserved for AVP */
#define TIMER3_OFFS  0x50  /* used as OS CPU event timer */
#define TIMER4_OFFS  0x58  /* reserved as LP2 wakeup trigger */

#define TIMER_TMR_PTV_0 0x0
#define TIMER_TMR_PCR_0 0x4

#define TIMERUS_OFFS 0x10
#define TIMERUS_CNTR_1US_0 0x0
#define TIMERUS_USEC_CFG_0 0x4

#define NSEC_PER_SEC	1000000000L

irqreturn_t tegra_clockevent_interrupt(int irq, void *dev_id, struct cpu_user_regs *regs)
{
	mmio_writel(1<<30, IO_ADDRESS(TEGRA_TMR1_BASE + TIMER3_OFFS) + TIMER_TMR_PCR_0);

        timer_tick(1);
#if 0
	mmio_writel( 0x80000000 | ((1000000/HZ)*((((NSEC_PER_SEC / HZ) * 16777) >> 24)+1)-1)
		, IO_ADDRESS(TEGRA_TMR1_BASE + TIMER3_OFFS) + TIMER_TMR_PTV_0);
#endif
#if defined(CONFIG_SMP) && !defined(CONFIG_LOCAL_TIMERS)
	smp_send_timer();
#endif
	return IRQ_HANDLED;
}

static struct irqaction tegra_clockevent_irq = {
	.name           = "Timer_event",
	.flags          = IRQF_DISABLED | IRQF_TIMER,
	.handler        = tegra_clockevent_interrupt,
};

static irqreturn_t tegra_lp2wake_interrupt(int irq, void *dev_id)
{
	mmio_writel(1<<30, IO_ADDRESS(TEGRA_TMR1_BASE + TIMER4_OFFS) + TIMER_TMR_PCR_0);
	return IRQ_HANDLED;
}

static struct irqaction tegra_lp2wake_irq = {
	.name		= "timer_lp2wake",
	.flags		= IRQF_DISABLED,
	.handler	= tegra_lp2wake_interrupt,
};

static unsigned long measure_input_freq(unsigned int *m, unsigned int *n)
{
	void *clk_rst = IO_ADDRESS(TEGRA_CLK_RESET_BASE);
	unsigned long osc = mmio_readl(clk_rst + CLK_RST_CONTROLLER_OSC_CTRL_0);
	osc >>= 30;

	switch (osc) {
	case 0: if (m && n) { *m=1; *n=13; } return 13000;
	case 1: if (m && n) { *m=5; *n=96; } return 19200;
	case 2: if (m && n) { *m=1; *n=12; } return 12000;
	case 3: if (m && n) { *m=1; *n=26; } return 26000;
	}
	return 0;
}

void tegra250_timer_init(void)
{
	void *tmr;
	unsigned int m, n;
	unsigned long val;
	u32 reg;
	
	tmr = IO_ADDRESS(TEGRA_TMR1_BASE + TIMERUS_OFFS);
	val = measure_input_freq(&m, &n);

	val = ((m-1)<<8) | (n-1);

	mmio_writel(val, tmr + TIMERUS_USEC_CFG_0);
#if 1
	mmio_writel(0, IO_ADDRESS(TEGRA_TMR1_BASE + TIMER3_OFFS)  + TIMER_TMR_PTV_0);
	
	reg = 0xc000270f;
	mmio_writel(reg, IO_ADDRESS(TEGRA_TMR1_BASE + TIMER3_OFFS) + TIMER_TMR_PTV_0);
#endif
	if (setup_irq(INT_TMR3, &tegra_clockevent_irq)) {
		printf("Failed to register clockevent IRQ\n");
		BUG();
	}
	if (setup_irq(INT_TMR4, &tegra_lp2wake_irq)) {
		printf("Failed to register LP2 wakeup timer IRQ\n");
		BUG();
	}
}
