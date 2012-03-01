/*
 * platform.c
 *
 * Copyright (C) 2008, 2009 Samsung Electronics 
 *         Sang-bum Suh <sbuk.suh@samsung.com>
 *         JaeMin Ryu   <jm77.ryu@samsung.com>
 *
 * Copyright (C) 2012 Sinby Corp.
 *         Ryos Suzuki ryos at sinby.com
 *
 * Secure Xen on ARM architecture designed by Sang-bum Suh consists of 
 * Xen on ARM and the associated access control.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public version 2 of License 
 * as published by the Free Software Foundation.
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

#include <xen/spinlock.h>
#include <xen/lib.h>
#include <xen/errno.h>
#include <xen/smp.h>
#include <asm/platform.h>
#include <asm/irq.h>
#include <asm/regs.h>
#include <asm/io.h>
#include <asm/arch/scu.h>

void zynq_uart_init(void);

// used by assembler utils.
void *gic_cpu_base_addr = 0;

//----------------------------------------------------------------
static
void zynq_platform_halt(int mode)
{
}
 
DECLARE_PLATFORM_OP(platform_halt, zynq_platform_halt);

//----------------------------------------------------------------
static
void zynq_memory_init(void)
{
	register_memory_bank(ZYNQ_DRAM_PHY_ADDR, ZYNQ_DRAM_SIZE);
}

//----------------------------------------------------------------
static
void zynq_irq_init()
{
	gic_cpu_base_addr = (void *)(IO_ADDRESS(ZYNQ_GIC_CPU_PHYS_BASE));

	gic_dist_init(0, IO_ADDRESS(ZYNQ_GIC_DIST_PHYS_BASE), INT_PPI_2_PTIMER);
	gic_cpu_init(0, IO_ADDRESS(ZYNQ_GIC_CPU_PHYS_BASE));
}

volatile int zynq_core_map = 1;

int wakeup_cpu(unsigned int cpu)
{
        zynq_core_map |= 1 <<  cpu;

        cpu_flush_cache_all();

	send_ipi(cpumask_of_cpu(cpu), 1);

        return 0;
}

void platform_secondary_init(unsigned int cpu)
{
#if 0
        gic_cpu_init(0, IO_ADDRESS(TEGRA_GIC_PROC_IF_BASE));
#endif
}

extern void secondary_startup(void);

static
void zynq_cpu1_init(void)
{
#if 0
	unsigned long r;
	unsigned long orig_reset;
	unsigned long loop;
	unsigned long ctrl;

	/* Initialize Snoop Control Unit */
	ctrl = mmio_readl(ZYNQ_SCU_PHYS_BASE + SCU_CONTROL_0);
	ctrl |= 1;
	mmio_writel(ctrl, ZYNQ_SCU_PHYS_BASE + SCU_CONTROL_0);

	/* Set boot entry */
	mmio_writel(virt_to_phys(secondary_startup),
	IO_ADDRESS(TEGRA_EXCEPTION_VECTORS_BASE) + EVP_CPU_RESET_VECTOR_0);

	dsb();
	isb();

	/* Halt CPU */
	mmio_writel(0, IO_ADDRESS(TEGRA_FLOW_CTRL_BASE) + FLOW_CTRL_HALT_CPUx_EVENTS(1));

	dsb();
	isb();

	/* CPU Clock Stop */
	r = mmio_readl(IO_ADDRESS(TEGRA_CLK_RESET_BASE) + CLK_RST_CONTROLLER_CLK_CPU_CMPLX_0);
	r &= ~CPU_CLK_STOP(1);
	mmio_writel(r, IO_ADDRESS(TEGRA_CLK_RESET_BASE) + CLK_RST_CONTROLLER_CLK_CPU_CMPLX_0);

	dsb();
	isb();

	/* Restart Slave CPU */
	mmio_writel(CPU_RESET(1), IO_ADDRESS(TEGRA_CLK_RESET_BASE) + CLK_RST_CONTROLLER_RST_CPU_CMPLX_CLR_0);

	dsb();
	isb();
#endif
}

struct platform platform_zynq = {
};


static struct platform *zynq_platform_setup(void *param)
{
	smp_init_cpus(1);

	zynq_cpu1_init();

	zynq_memory_init();

	zynq_uart_init();

#ifdef TEST
	init_console();
	printk("Hello World\n");
#endif

	zynq_irq_init();

	zynq_timer_init();

	return &platform_zynq;
}

DECLARE_PLATFORM_OP(platform_setup, zynq_platform_setup);
