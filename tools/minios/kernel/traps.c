#include <os/os.h>
#include <os/traps.h>
#include <os/hypervisor.h>
#include <os/lib.h>

/*
 * These are assembler stubs in entry.S.
 * They are the actual entry points for virtual exceptions.
 */
void reset(int nop)
{
    printk("reset\n");
}

void undefined_instruction(int nop)
{
	printk("undefined_instruction\n");
}

void swi_dispatch(int vector)
{
	printk("swi_dispatch\n");
}

void prefetch_abort(int nop)
{
	printk("prefetch_abort\n");
}

void data_abort(int nop)
{
	printk("data_abort\n");
}

void irq_dispatch(int vector)
{
	printk("irq_dispatch\n");
}

void fiq_dispatch(int vector)
{
	printk("fiq_dispatch\n");
}

/*
 * Submit a virtual IDT to teh hypervisor. This consists of tuples
 * (interrupt vector, privilege ring, CS:EIP of handler).
 * The 'privilege ring' field specifies the least-privileged ring that
 * can trap to that vector using a software-interrupt instruction (INT).
 */
static trap_info_t trap_table[] = {
    {  0, (unsigned long)reset},
    {  1, (unsigned long)undefined_instruction},
    {  2, (unsigned long)swi_dispatch},
    {  3, (unsigned long)prefetch_abort},
    {  4, (unsigned long)data_abort},
    {  6, (unsigned long)irq_dispatch},
    {  7, (unsigned long)fiq_dispatch}
};

void trap_init(void)
{
    HYPERVISOR_set_trap_table(trap_table); 
}

