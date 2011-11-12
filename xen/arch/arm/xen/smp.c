/*
 *  linux/arch/arm/kernel/smp.c
 *
 *  Copyright (C) 2002 ARM Limited, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <xen/linkage.h>
#include <xen/spinlock.h>
#include <xen/cpumask.h>
#include <xen/smp.h>
#include <xen/irq.h>
#include <xen/softirq.h>
#include <xen/sched.h>
#include <xen/preempt.h>
#include <asm/mmu.h>
#include <asm/current.h>
#include <asm/cpu.h>
#include <asm/delay.h>

extern struct domain *idle_domain;

enum ipi_msg_type {
	IPI_TIMER,
	IPI_SCHEDULE,
	IPI_RPC,
	IPI_CPU_STOP,
};

cpumask_t cpu_online_map;
cpumask_t cpu_present_map;
cpumask_t cpu_possible_map;

cpumask_t cpu_sibling_map[NR_CPUS];
cpumask_t cpu_core_map[NR_CPUS];

void smp_cpu_sibling_map_setup(unsigned int cpu)
{
	unsigned int i;

	for_each_cpu_mask(i, cpu_present_map) {
		cpu_set(i, cpu_sibling_map[cpu]);
		cpu_set(cpu, cpu_sibling_map[i]);

		cpu_set(i, cpu_core_map[cpu]);
		cpu_set(cpu, cpu_core_map[i]);
	}
}

void smp_prepare_cpus(unsigned int max_cpus)
{
	unsigned int i;
	unsigned int cpu;
	unsigned long timeout;

        for_each_present_cpu(cpu)
	{
		if (cpu >= max_cpus)
		{
			printk("Miss configuration\n");
			return;
		}
		
                if (cpu_online(cpu))  {
			smp_cpu_sibling_map_setup(cpu);
			continue;
		}

		wakeup_cpu(cpu);

		timeout = jiffies + HZ;
		while(time_before(jiffies, timeout)) {
			if (cpu_online(cpu)) {
				smp_cpu_sibling_map_setup(cpu);
				break;
			}
			udelay(10);
			dmb();
		}
	}
}


void prepare_boot_cpu(unsigned int boot_cpu_id)
{
	cpu_set(boot_cpu_id, cpu_online_map);
	cpu_set(boot_cpu_id, cpu_present_map);
	cpu_set(boot_cpu_id, cpu_possible_map);

	cpu_info_init(current_cpu_info());
}

asmlinkage void secondary_cpu_boot_entry(void)
{
	unsigned int cpu;
	struct vcpu *vcpu;
	struct cpu_info *cpu_info;

	cpu = hard_smp_processor_id();
	printk("Startup CPU #%d\n", cpu);
	/*
	 * Give the platform a chance to do its own initialisation.
	 */
	platform_secondary_init(cpu);

	vcpu = alloc_vcpu(idle_domain, cpu, cpu);

	BUG_ON(vcpu == NULL);

	vcpu->arch.ctx.ttbr0 = (get_ttbr() & TTB_MASK);

	cpu_info = current_cpu_info();
	cpu_info_init(cpu_info);

	set_current_vcpu(vcpu);

	cpu_set(cpu, cpu_online_map);

	local_irq_enable();
	local_fiq_enable();

	start_idle_loop();
}

static void send_ipi_message(cpumask_t callmap, enum ipi_msg_type msg)
{
	send_ipi(callmap, msg);
}

void smp_send_event_check_mask(cpumask_t mask)
{
        cpu_clear(smp_processor_id(), mask);

        if (!cpus_empty(mask))
                send_ipi_message(mask, IPI_SCHEDULE);
}

static void ipi_timer(void)
{

	send_timer_event(current);

        raise_softirq(TIMER_SOFTIRQ);
}

asmlinkage void do_IPI(unsigned int ipi)
{
        switch(ipi) {
                case IPI_TIMER :
                        ipi_timer();
                        break;
                case IPI_SCHEDULE:
			raise_softirq(SCHEDULE_SOFTIRQ);
                        break;
                case IPI_CPU_STOP:
			while(1);
                        break;
        }
}

void smp_send_timer(void)
{
	cpumask_t mask = cpu_online_map;
	cpu_clear(smp_processor_id(), mask);
	send_ipi_message(mask, IPI_TIMER);
}

/*
 *  * Initialise the CPU possible map early - this describes the CPUs
 *   * which may be present or become present in the system.
 *    */
void smp_init_cpus(unsigned long nr_cores)
{
	unsigned int i;

	/* sanity check */
	if (nr_cores == 0) {
		printk("# of cores less than 1\n");

		nr_cores = 1;
	}

	if (nr_cores > NR_CPUS) {
		printk("# of cores (%d) greater than %d", nr_cores, NR_CPUS);
		nr_cores = NR_CPUS;
	}

	for (i = 0; i < nr_cores; i++) {
		cpu_set(i, cpu_possible_map);
		cpu_set(i, cpu_present_map);
	}
}

