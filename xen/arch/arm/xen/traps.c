/*
 * traps.c
 *
 * Copyright (C) 2008-2011 Samsung Electronics
 *          Sang-bum Suh <sbuk.suh@samsung.com>
 *          JaeMin Ryu   <jm77.ryu@samsung.com>
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

#include <xen/config.h>
#include <xen/linkage.h>
#include <xen/compile.h>
#include <xen/domain_page.h>
#include <xen/init.h>
#include <xen/sched.h>
#include <xen/lib.h>
#include <xen/console.h>
#include <xen/mm.h>
#include <xen/irq.h>
#include <xen/symbols.h>
#include <asm/current.h>
#include <asm/processor.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/memory.h>

asmlinkage void __div0(void)
{
        printk("Division by zero in kernel.\n");
}

int fixup_exception(struct cpu_ctx *regs)
{
	unsigned long fixup;

	regs->pc = search_extable(regs->pc);

	return fixup;
}

void show_registers(struct cpu_ctx *ctx)
{
	unsigned long flags = condition_codes(ctx);

	printk("CPU: %d\n", smp_processor_id());
	printk("pc : [<%08lx>] \n", ctx->pc);
	printk("slr: %08lx  ssp: %08lx\n", ctx->slr, ctx->ssp);
	printk("ulr: %08lx  usp: %08lx\n", ctx->ulr, ctx->usp);
	printk("ip : %08lx  fp : %08lx\n", ctx->r12, ctx->r11);
	printk("r10: %08lx  r9 : %08lx  r8 : %08lx\n",
		ctx->r10, ctx->r9, ctx->r8);
	printk("r7 : %08lx  r6 : %08lx  r5 : %08lx  r4 : %08lx\n",
		ctx->r7, ctx->r6, ctx->r5, ctx->r4);
	printk("r3 : %08lx  r2 : %08lx  r1 : %08lx  r0 : %08lx\n",
		ctx->r3, ctx->r2, ctx->r1, ctx->r0);
	printk("Flags: %c%c%c%c",
		flags & PSR_N_BIT ? 'N' : 'n',
		flags & PSR_Z_BIT ? 'Z' : 'z',
		flags & PSR_C_BIT ? 'C' : 'c',
		flags & PSR_V_BIT ? 'V' : 'v');
	printk("  IRQs o%s  FIQs o%s  Mode %s%s\n",
	interrupts_enabled(ctx) ? "n" : "ff",
	fast_interrupts_enabled(ctx) ? "n" : "ff",
	thumb_mode(ctx) ? " (T)" : "");
}

static int verify_stack(unsigned long sp)
{
	if (sp < HYPERVISOR_VIRT_START) {
		return -EFAULT;
	}

	return 0;
}

static void backtrace(struct cpu_ctx *ctx)
{
	int ok = 1;
	unsigned long fp;

	printk("Backtrace: ");
	fp = ctx->r12;
	if(!fp) {
		printk("no frame pointer");
		ok = 0;
	}
	else if (verify_stack(fp)) {
		printk("invalid frame pointer 0x%08x", fp);
		ok = 0;
	}
#if 0
	else if (fp < (unsigned long)end_of_stak(tsk))
		printk("frame pointer underflow");
#endif
	printk("\n");

	if (ok)
		c_backtrace(fp, ctx->spsr);
}

static void unrecoverable_fault(const char *str, int err, struct vcpu *v, struct cpu_ctx *ctx)
{
	printk("Unrecoverable Fault : %s\n", str);

	show_registers(ctx);
	backtrace(ctx);

	while(1);

}

int is_kernel_text(unsigned long addr)
{
	extern char _stext, _etext;
	if (addr >= (unsigned long) &_stext &&
	    addr <= (unsigned long) &_etext)
	    return 1;
	return 0;
}

unsigned long kernel_text_end(void)
{
	extern char _etext;
	return (unsigned long) &_etext;
}

long do_set_callbacks(unsigned long event, unsigned long failsafe)
{
	struct vcpu *v = (struct vcpu *)current;
    
	if ((event < HYPERVISOR_VIRT_START) && (failsafe < HYPERVISOR_VIRT_START)) {
		v->arch.ctx.event_callback    = event;
		v->arch.ctx.failsafe_callback = failsafe;

		return 0;
	}

	printk("Invalid hypervisor callback address = 0x%x, 0x%x\n", event, failsafe);

	return -EINVAL;

}

asmlinkage void do_prefetch_abort(unsigned long pc, struct cpu_ctx *regs)
{
	while(1);
	unrecoverable_fault("prefetch abort", 0, current, regs);
}

asmlinkage void do_data_abort(unsigned long fsr, unsigned long far, struct cpu_ctx*regs)
{
	while(1);
	unrecoverable_fault("data abort", 0, current, regs);
}

asmlinkage void do_undefined_instruction(unsigned long pc, struct cpu_ctx *regs)
{
	while(1);
	unrecoverable_fault("undefined instruction", 0, current, regs);
}

long do_set_trap_table(XEN_GUEST_HANDLE(trap_info_t) traps)
{
	int i;
	unsigned long trap_table;

	if ( guest_handle_is_null(traps) )
		goto failed;

	trap_table = (unsigned long)traps.p;

	current->arch.ctx.vbar = trap_table;

	return 0;

failed:
	current->arch.ctx.vbar = 0;

	printk("Trap table install failed\n");

	return -EFAULT;
}

extern pte_t *exception_table_vector;

void trap_init(void)
{
	extern char exception_vectors_table[];
	int idx;

	idx = PGT_IDX(VECTORS_BASE);

	exception_table_vector[idx] = MK_PTE(MEMMAP_HYPERVISOR_BASE, PTE_VECTOR_PAGE);
	pte_sync(&exception_table_vector[idx]);

	memcpy((void *)VECTORS_BASE, (void *)exception_vectors_table, sizeof(unsigned long) * 16);

	cpu_flush_cache_range(VECTORS_BASE, VECTORS_BASE + PAGE_SIZE, 0);

}

