/******************************************************************************
 * keyhandler.c
 */

#include <asm/regs.h>
#include <xen/keyhandler.h> 
#include <xen/reboot.h>
#include <xen/event.h>
#include <xen/console.h>
#include <xen/serial.h>
#include <xen/sched.h>
#include <xen/softirq.h>
#include <xen/domain.h>
#include <xen/rangeset.h>
#include <asm/debugger.h>
#include <asm/shadow.h>
#include <asm/div64.h>

#define KEY_MAX 256
#define STR_MAX  64

static struct {
    union {
        keyhandler_t     *handler;
        irq_keyhandler_t *irq_handler;
    } u;
    unsigned int flags;
    char         desc[STR_MAX]; 
} key_table[KEY_MAX]; 

#define KEYHANDLER_IRQ_CALLBACK 0x1

static unsigned char keypress_key;

static void keypress_softirq(void)
{
    keyhandler_t *h;
    unsigned char key = keypress_key;
    if ( (h = key_table[key].u.handler) != NULL )
        (*h)(key);
}

void handle_keypress(unsigned char key, struct cpu_user_regs *regs)
{
    irq_keyhandler_t *h;

    if ( key_table[key].flags & KEYHANDLER_IRQ_CALLBACK )
    {
        if ( (h = key_table[key].u.irq_handler) != NULL )
            (*h)(key, regs);
    }
    else
    {
        keypress_key = key;
        raise_softirq(KEYPRESS_SOFTIRQ);
    }
}

void register_keyhandler(
    unsigned char key, keyhandler_t *handler, char *desc)
{
    ASSERT(key_table[key].u.handler == NULL);
    key_table[key].u.handler = handler;
    key_table[key].flags     = 0;
    strncpy(key_table[key].desc, desc, STR_MAX);
    key_table[key].desc[STR_MAX-1] = '\0'; 
}

void register_irq_keyhandler(
    unsigned char key, irq_keyhandler_t *handler, char *desc)
{
    ASSERT(key_table[key].u.irq_handler == NULL);
    key_table[key].u.irq_handler = handler;
    key_table[key].flags         = KEYHANDLER_IRQ_CALLBACK;
    strncpy(key_table[key].desc, desc, STR_MAX);
    key_table[key].desc[STR_MAX-1] = '\0'; 
}

static void show_handlers(unsigned char key)
{
    int i; 
    printk("'%c' pressed -> showing installed handlers\n", key);
    for ( i = 0; i < KEY_MAX; i++ ) 
        if ( key_table[i].u.handler != NULL ) 
            printk(" key '%c' (ascii '%02x') => %s\n", 
                   (i<33 || i>126)?(' '):(i),i,
                   key_table[i].desc);
}

static void dump_registers(unsigned char key, struct cpu_user_regs *regs)
{
    printk("'%c' pressed -> dumping registers\n", key); 
    show_registers(regs); 
}

static void halt_machine(unsigned char key, struct cpu_user_regs *regs)
{
    printk("'%c' pressed -> rebooting machine\n", key); 
    machine_restart(NULL); 
}

static void cpuset_print(char *set, int size, cpumask_t mask)
{
    *set++ = '{';
    set += cpulist_scnprintf(set, size-2, mask);
    *set++ = '}';
    *set++ = '\0';
}

static void dump_domains(unsigned char key)
{
    struct domain *d;
    struct vcpu   *v;
    s_time_t       now = NOW();
    char           cpuset[100];

    printk("'%c' pressed -> dumping domain info (now=0x%X:%08X)\n", key,
           (u32)(now>>32), (u32)now); 

    read_lock(&domlist_lock);

    for_each_domain ( d )
    {
        printk("General information for domain %u:\n", d->domain_id);
        cpuset_print(cpuset, sizeof(cpuset), d->domain_dirty_cpumask);
        printk("    flags=%lx refcnt=%d nr_pages=%d xenheap_pages=%d "
               "dirty_cpus=%s\n",
               d->domain_flags, atomic_read(&d->refcnt),
               d->tot_pages, d->xenheap_pages, cpuset);
        printk("    handle=%02x%02x%02x%02x-%02x%02x-%02x%02x-"
               "%02x%02x-%02x%02x%02x%02x%02x%02x\n",
               d->handle[ 0], d->handle[ 1], d->handle[ 2], d->handle[ 3],
               d->handle[ 4], d->handle[ 5], d->handle[ 6], d->handle[ 7],
               d->handle[ 8], d->handle[ 9], d->handle[10], d->handle[11],
               d->handle[12], d->handle[13], d->handle[14], d->handle[15]);

        arch_dump_domain_info(d);

        rangeset_domain_printk(d);

        dump_pageframe_info(d);
               
        printk("VCPU information and callbacks for domain %u:\n",
               d->domain_id);
        for_each_vcpu ( d, v ) {
            printk("    VCPU%d: CPU%d [has=%c] flags=%lx "
                   "upcall_pend = %02x, upcall_mask = %02x ",
                   v->vcpu_id, v->processor,
                   test_bit(_VCPUF_running, &v->vcpu_flags) ? 'T':'F',
                   v->vcpu_flags,
                   v->vcpu_info->evtchn_upcall_pending, 
                   v->vcpu_info->evtchn_upcall_mask);
            cpuset_print(cpuset, sizeof(cpuset), v->vcpu_dirty_cpumask);
            printk("dirty_cpus=%s ", cpuset);
            cpuset_print(cpuset, sizeof(cpuset), v->cpu_affinity);
            printk("cpu_affinity=%s\n", cpuset);
            printk("    Notifying guest (virq %d, port %d, stat %d/%d/%d)\n",
                   VIRQ_DEBUG, v->virq_to_evtchn[VIRQ_DEBUG],
                   test_bit(v->virq_to_evtchn[VIRQ_DEBUG], 
                            &d->shared_info->evtchn_pending[0]),
                   test_bit(v->virq_to_evtchn[VIRQ_DEBUG], 
                            &d->shared_info->evtchn_mask[0]),
                   test_bit(v->virq_to_evtchn[VIRQ_DEBUG]/BITS_PER_LONG, 
                            &v->vcpu_info->evtchn_pending_sel));
            send_guest_virq(v, VIRQ_DEBUG);
        }
    }

    read_unlock(&domlist_lock);
}

static cpumask_t read_clocks_cpumask = CPU_MASK_NONE;
static s_time_t read_clocks_time[NR_CPUS];

static void read_clocks_slave(void *unused)
{
    unsigned int cpu = smp_processor_id();
    while ( !cpu_isset(cpu, read_clocks_cpumask) )
        cpu_relax();
    read_clocks_time[cpu] = NOW();
    cpu_clear(cpu, read_clocks_cpumask);
}

static void read_clocks(unsigned char key)
{
    unsigned int cpu = smp_processor_id(), min_cpu, max_cpu;
    u64 min, max, dif, difus;
    static DEFINE_SPINLOCK(lock);

    spin_lock(&lock);

    smp_call_function(read_clocks_slave, NULL, 0, 0);

    local_irq_disable();
    read_clocks_cpumask = cpu_online_map;
    read_clocks_time[cpu] = NOW();
    cpu_clear(cpu, read_clocks_cpumask);
    local_irq_enable();

    while ( !cpus_empty(read_clocks_cpumask) )
        cpu_relax();

    min_cpu = max_cpu = cpu;
    for_each_online_cpu ( cpu )
    {
        if ( read_clocks_time[cpu] < read_clocks_time[min_cpu] )
            min_cpu = cpu;
        if ( read_clocks_time[cpu] > read_clocks_time[max_cpu] )
            max_cpu = cpu;
    }

    min = read_clocks_time[min_cpu];
    max = read_clocks_time[max_cpu];

    spin_unlock(&lock);

    dif = difus = max - min;
    do_div(difus, 1000);
    printk("Min = %"PRIu64" ; Max = %"PRIu64" ; Diff = %"PRIu64
           " (%"PRIu64" microseconds)\n",
           min, max, dif, difus);
}

extern void dump_runq(unsigned char key);
#ifndef NDEBUG
extern void audit_domains_key(unsigned char key);
#endif

#ifdef PERF_COUNTERS
extern void perfc_printall(unsigned char key);
extern void perfc_reset(unsigned char key);
#endif

static void do_debug_key(unsigned char key, struct cpu_user_regs *regs)
{
    (void)debugger_trap_fatal(0xf001, regs);
    nop(); /* Prevent the compiler doing tail call
                             optimisation, as that confuses xendbg a
                             bit. */
}

#ifndef NDEBUG
static void debugtrace_key(unsigned char key)
{
    debugtrace_send_to_console = !debugtrace_send_to_console;
    debugtrace_dump();
    printk("debugtrace_printk now writing to %s.\n",
           debugtrace_send_to_console ? "console" : "buffer");
}
#endif

void initialize_keytable(void)
{
    open_softirq(KEYPRESS_SOFTIRQ, keypress_softirq);

    register_irq_keyhandler(
        'd', dump_registers, "dump registers"); 
    register_keyhandler(
        'h', show_handlers, "show this message");
    register_keyhandler(
        'q', dump_domains, "dump domain (and guest debug) info");
    register_keyhandler(
        'r', dump_runq,      "dump run queues");
    register_irq_keyhandler(
        'R', halt_machine,   "reboot machine"); 

    register_keyhandler(
        't', read_clocks, "display multi-cpu clock info");

#ifndef NDEBUG
    register_keyhandler(
        'o', audit_domains_key,  "audit domains >0 EXPERIMENTAL");
    register_keyhandler(
        'T', debugtrace_key, "toggle debugtrace to console/buffer");
#endif

#ifdef PERF_COUNTERS
    register_keyhandler(
        'p', perfc_printall, "print performance counters"); 
    register_keyhandler(
        'P', perfc_reset,    "reset performance counters"); 
#endif

    register_irq_keyhandler('%', do_debug_key,   "Trap to xendbg");
}

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
