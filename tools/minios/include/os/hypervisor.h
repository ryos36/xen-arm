/*
 * hypervisor.h
 * 
 * Hypervisor handling.
 * 
 */

#ifndef _HYPERVISOR_H_
#define _HYPERVISOR_H_

#include <xen/interface/types.h>
#include <xen/interface/xen.h>
#include <xen/interface/dom0_ops.h>
#include <xen/interface/sched.h>

#include <os/os.h>
#include <os/traps.h>

#define HYPERCALL_NO(x)                      # x
#define HYPERCALL_INSTR(x)                  "swi " HYPERCALL_NO(x)

#define DECLARE_REG(name, reg)				register long name __asm(reg)
#define ASSIGN_REG(name, reg, value)		DECLARE_REG(name, reg) = value;

#define __SYS_REG_LIST(regs...) regs

static inline int HYPERVISOR_set_trap_table(trap_info_t *table)
{
	register long a0 __asm__("r0") = (long)(table);
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_set_trap_table)
		: "=r"(r0)
		: "0"(a0)
		: "memory");
		
	return (int)r0;
}

static inline int HYPERVISOR_mmu_update(mmu_update_t *req, int count, int *success_count, domid_t domid)
{
	register long a0 __asm__("r0") = (long)(req);
	register long a1 __asm__("r1") = (long)(count);
	register long a2 __asm__("r2") = (long)(success_count);
	register long a3 __asm__("r3") = (long)domid;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_mmu_update)
		: "=r"(r0)
		: "0"(a0), "r"(a1), "r"(a2), "r"(a3)
		: "memory");
		
	return (int)r0;
}

static inline int HYPERVISOR_mmuext_op(struct mmuext_op *op, int count, int *success_count, domid_t domid)
{
	register long a0 __asm__("r0") = (long)op;
	register long a1 __asm__("r1") = (long)count;
	register long a2 __asm__("r2") = (long)success_count;
	register long a3 __asm__("r3") = (long)domid;
	
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_mmuext_op)
		: "=r"(r0)
		: "0"(a0), "r"(a1), "r"(a2), "r"(a3)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_stack_switch(unsigned long ss, unsigned long esp)
{
	register long a0 __asm__("r0") = (long)ss;
	register long a1 __asm__("r1") = (long)esp;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_stack_switch)
		: "=r"(r0)
		: "0"(a0), "r"(a1)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_set_callbacks
(
	unsigned long event_address,
	unsigned long failsafe_address
)
{
	register long a0 __asm__("r0") = (long)event_address;
	register long a1 __asm__("r1") = (long)failsafe_address;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_set_callbacks)
		: "=r"(r0)
		: "0"(a0), "r"(a1)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_fpu_taskswitch(int set)
{
	register long a0 __asm__("r0") = (long)set;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_fpu_taskswitch)
		: "=r"(r0)
		: "0"(a0)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_sched_op(int cmd, unsigned long arg)
{
	register long a0 __asm__("r0") = (long)cmd;
	register long a1 __asm__("r1") = (long)arg;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_sched_op)
		: "=r"(r0)
		: "0"(a0), "r"(a1)
		: "memory");
	
	return (int)r0;
}

static inline long HYPERVISOR_set_timer_op(u64 timeout)
{
	register long a0 __asm__("r0") = (unsigned long)(timeout>>32);
	register long a1 __asm__("r1") = (unsigned long)timeout;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_set_timer_op)
		: "=r"(r0)
		: "0"(a0), "r"(a1)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_dom0_op(dom0_op_t *dom0_op)
{
	register long a0 __asm__("r0") = (unsigned long)(dom0_op);
	register long r0 __asm__("r0");
	
	dom0_op->interface_version = DOM0_INTERFACE_VERSION;
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_dom0_op)
		: "=r"(r0)
		: "0"(a0)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_memory_op(unsigned int cmd, void *arg)
{
	register long a0 __asm__("r0") = (long)cmd;
	register long a1 __asm__("r1") = (long)arg;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_memory_op)
		: "=r"(r0)
		: "0"(a0), "r"(a1)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_multicall(void *call_list, int nr_calls)
{
	register long a0 __asm__("r0") = (long)call_list;
	register long a1 __asm__("r1") = (long)nr_calls;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_multicall)
		: "=r"(r0)
		: "0"(a0), "r"(a1)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_update_va_mapping(unsigned long va, unsigned long new_val, unsigned long flags)
{
        register long a0 __asm__("r0") = (long)va;
        register long a1 __asm__("r1") = (long)flags;
        register long a2 __asm__("r2") = (long)new_val;
        register long a3 __asm__("r3") = (long)0;

	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_update_va_mapping)
		: "=r"(r0)
		: "0"(a0), "r"(a1), "r"(a2), "r"(a3)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_event_channel_op(void *op)
{
	register long a0 __asm__("r0") = (long)op;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_event_channel_op)
		: "=r"(r0)
		: "0"(a0)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_xen_version(int cmd, void *arg)
{
	register long a0 __asm__("r0") = (long)cmd;
	register long a1 __asm__("r1") = (long)arg;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_xen_version)
		: "=r"(r0)
		: "0"(a0), "r"(a1)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_console_io(int cmd, int count, char *str)
{
	register long a0 __asm__("r0") = cmd;
	register long a1 __asm__("r1") = count;
	register long a2 __asm__("r2") = (long)str;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_console_io)
		: "=r"(r0)
		: "0"(a0), "r"(a1), "r"(a2)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_physdev_op(void *physdev_op)
{
	register long a0 __asm__("r0") = (long)physdev_op;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_physdev_op)
		: "=r"(r0)
		: "0"(a0)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_grant_table_op(unsigned int cmd, void *uop, unsigned int count)
{
	register long a0 __asm__("r0") = cmd;
	register long a1 __asm__("r1") = (long)uop;
	register long a2 __asm__("r2") = count;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_grant_table_op)
		: "=r"(r0)
		: "0"(a0), "r"(a1), "r"(a2)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_update_va_mapping_otherdomain
(
	unsigned long va, 
	pte_t new_val, 
	unsigned long flags, 
	domid_t domid
)
{
	register long a0 __asm__("r0") =(long)va;
	register long a1 __asm__("r1") = (long)new_val.pte_low;
	register long a2 __asm__("r2") = 0;
	register long a3 __asm__("r3") = (long)flags;
	register long a4 __asm__("r4") = (long)domid;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_vcpu_op)
		: "=r"(r0)
		: "0"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4)
		: " memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_vm_assist(unsigned int cmd, unsigned int type)
{
	register long a0 __asm__("r0") = (long)cmd;
	register long a1 __asm__("r1") = (long)type;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_vm_assist)
		: "=r"(r0)
		: "0"(a0), "r"(a1)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_vcpu_op(int cmd, int vcpuid, void *extra_args)
{
	register long a0 __asm__("r0") = cmd;
	register long a1 __asm__("r1") = vcpuid;
	register long a2 __asm__("r2") = (long)extra_args;
	register long r0 __asm__("r0");
	
	__asm__ __volatile__(
		HYPERCALL_INSTR(__HYPERVISOR_vcpu_op)
		: "=r"(r0)
		: "0"(a0), "r"(a1), "r"(a2)
		: "memory");
	
	return (int)r0;
}

static inline int HYPERVISOR_suspend(unsigned long srec)
{
	return 0;
}
#endif
