/*
 *  copied from linux/arch/arm/common/gic.c
 *
 *  Copyright (C) 2002 ARM Limited, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Interrupt architecture for the GIC:
 *
 * o There is one Interrupt Distributor, which receives interrupts
 *   from system devices and sends them to the Interrupt Controllers.
 *
 * o There is one CPU Interface per CPU, which sends interrupts sent
 *   by the Distributor, and interrupts generated locally, to the
 *   associated CPU. The base address of the CPU interface is usually
 *   aliased so that the same address points to different chips depending
 *   on the CPU it is accessed from.
 *
 * Note that IRQs 0-31 are special - they are local to each CPU.
 * As such, the enable set/clear, pending set/clear and active bit
 * registers are banked per-cpu for these sources.
 */
#include <xen/config.h>
#include <xen/init.h>
#include <xen/types.h>
#include <xen/cpumask.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/gic.h>

extern struct irqdesc irq_desc[NR_IRQS];
static struct irqchip gic_chip;
void set_irq_chip(unsigned int irq, struct irqchip *chip);
int set_irq_chip_data(unsigned int irq, void *data);

static spinlock_t irq_controller_lock = SPIN_LOCK_UNLOCKED;

struct gic_chip_data {
        unsigned int irq_offset;
        void *dist_base;
        void *cpu_base;
};

#ifndef MAX_GIC_NR
#define MAX_GIC_NR      1
#endif

static struct gic_chip_data gic_data[MAX_GIC_NR];

static inline void *gic_dist_base(unsigned int irq)
{
        struct gic_chip_data *gic_data = get_irq_chip_data(irq);
        return gic_data->dist_base;
}

static inline void *gic_cpu_base(unsigned int irq)
{
        struct gic_chip_data *gic_data = get_irq_chip_data(irq);
        return gic_data->cpu_base;
}

static inline unsigned int gic_irq(unsigned int irq)
{
        struct gic_chip_data *gic_data = get_irq_chip_data(irq);
        return irq - gic_data->irq_offset;
}
void gic_ack_irq(unsigned int irq)
{
        u32 mask = 1 << (irq % 32);

        spin_lock(&irq_controller_lock);
        mmio_writel(mask, gic_dist_base(irq) + ICDICER + (gic_irq(irq) / 32) * 4);
        mmio_writel(gic_irq(irq), gic_cpu_base(irq) + ICCEOIR);
        spin_unlock(&irq_controller_lock);
}

void gic_mask_irq(unsigned int irq)
{
        u32 mask = 1 << (irq % 32);

        spin_lock(&irq_controller_lock);
        mmio_writel(mask, gic_dist_base(irq) + ICDICER + (gic_irq(irq) / 32) * 4);
        spin_unlock(&irq_controller_lock);
}

void gic_unmask_irq(unsigned int irq)
{
        u32 mask = 1 << (irq % 32);

        spin_lock(&irq_controller_lock);
        mmio_writel(mask, gic_dist_base(irq) + ICDISER + (gic_irq(irq) / 32) * 4);
        spin_unlock(&irq_controller_lock);
}

#ifdef CONFIG_SMP
void gic_set_cpu(unsigned int irq, cpumask_t mask_val)
{
        void *reg = gic_dist_base(irq) + ICDIPTR + (gic_irq(irq) & ~3);
        unsigned int shift = (irq % 4) * 8;
        unsigned int cpu = first_cpu(mask_val);
        u32 val;

        spin_lock(&irq_controller_lock);
        irq_desc[irq].cpu = cpu;
        val = mmio_readl(reg) & ~(0xff << shift);
        val |= 1 << (cpu + shift);
        mmio_writel(val, reg);
        spin_unlock(&irq_controller_lock);
}
#endif

void gic_handle_cascade_irq(unsigned int irq, struct irqdesc *desc, struct cpu_user_regs *regs)
{
}
static struct irqchip gic_chip = {
        .trigger_type   = "level",
        .ack            = gic_ack_irq,
        .mask           = gic_mask_irq,
        .unmask         = gic_unmask_irq,
#ifdef CONFIG_SMP
        .set_affinity   = gic_set_cpu,
#endif
};

void gic_cascade_irq(unsigned int gic_nr, unsigned int irq)
{
}

void gic_dist_init(unsigned int gic_nr, void *base, unsigned int irq_start)
{
        unsigned int max_irq, i;
//      u32 cpumask = 1 << smp_processor_id();
        u32 cpumask = 1 << 0;

        if (gic_nr >= MAX_GIC_NR) {
                BUG();
	}

        cpumask |= cpumask << 8;
        cpumask |= cpumask << 16;

        gic_data[gic_nr].dist_base = base;
        gic_data[gic_nr].irq_offset = (irq_start - 1) & ~31;

        mmio_writel(0, base + ICDDCR);

        /*
         * Find out how many interrupts are supported.
         */
        max_irq = mmio_readl(base + ICDICTR) & 0x1f;
        max_irq = (max_irq + 1) * 32;

        /*
         * The GIC only supports up to 1020 interrupt sources.
         * Limit this to either the architected maximum, or the
         * platform maximum.
         */
        if (max_irq > max(1020, NR_IRQS)) {
                max_irq = max(1020, NR_IRQS);
	}

        /*
         * Set all global interrupts to be level triggered, active low.
         */
        for (i = 32; i < max_irq; i += 16) {
                mmio_writel(0, base + ICDICFR + i * 4 / 16);
	}

        /*
         * Set all global interrupts to this CPU only.
         */
        for (i = 32; i < max_irq; i += 4) {
                mmio_writel(cpumask, base + ICDIPTR + i * 4 / 4);
	}

        /*
         * Set priority on all interrupts.
         */
        for (i = 0; i < max_irq; i += 4) {
                mmio_writel(0xa0a0a0a0, base + ICDIPR + i * 4 / 4);
	}

        /*
         * Disable all interrupts.
         */
        for (i = 0; i < max_irq; i += 32) {
                mmio_writel(0xffffffff, base + ICDICER + i * 4 / 32);
	}

        /*
         * Setup the Linux IRQ subsystem.
         */
        for (i = irq_start; i < max_irq; i++) {
		uint32_t int_config_field;
                set_irq_chip(i, &gic_chip);
                set_irq_chip_data(i, &gic_data[gic_nr]);
		int_config_field = mmio_readl(base + ICDICFR + i * 4 / 16);
		int_config_field >>= (((i % 16) * 2) + 1);
		int_config_field &= 0x1;
		if ( int_config_field ) {
			set_irq_handler(i, edge_irq_handler);
		} else {
			set_irq_handler(i, level_irq_handler);
		}
                set_irq_flags(i, IRQF_VALID);
        }

        mmio_writel(1, base + ICDDCR);
}

void gic_cpu_init(unsigned int gic_nr, void *base)
{
        if (gic_nr >= MAX_GIC_NR) {
                BUG();
	}

        gic_data[gic_nr].cpu_base = base;

        mmio_writel(0xf0, base + ICCPMR);
        mmio_writel(1, base + ICCICR);
}

void gic_raise_softirq(cpumask_t cpumask, unsigned int irq)
{
        unsigned long map = *cpus_addr(cpumask);

        /* this always happens on GIC0 */
        mmio_writel(map << 16 | irq, gic_data[0].dist_base + ICDSGIR);
}

