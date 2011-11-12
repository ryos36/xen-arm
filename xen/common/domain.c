/******************************************************************************
 * domain.c
 * 
 * Generic domain-handling functions.
 */

#include <xen/config.h>
#include <xen/init.h>
#include <xen/lib.h>
#include <xen/errno.h>
#include <xen/sched.h>
#include <xen/domain.h>
#include <xen/mm.h>
#include <xen/event.h>
#include <xen/time.h>
#include <xen/console.h>
#include <xen/softirq.h>
#include <xen/domain_page.h>
#include <xen/rangeset.h>
#include <xen/guest_access.h>
#include <xen/hypercall.h>
#include <xen/delay.h>
#include <asm/debugger.h>
#include <public/dom0_ops.h>
#include <public/sched.h>
#include <public/vcpu.h>

/* Both these structures are protected by the domlist_lock. */
rwlock_t domlist_lock = RW_LOCK_UNLOCKED;
struct domain *domain_hash[DOMAIN_HASH_SIZE];
struct domain *domain_list;

struct domain *dom0;

struct domain *domain_create(domid_t dom_id, unsigned int cpu)
{
    struct domain *d, **pd;
    struct vcpu *v;

    if ( (d = alloc_domain()) == NULL )
        return NULL;

    d->domain_id = dom_id;

    atomic_set(&d->refcnt, 1);

    spin_lock_init(&d->big_lock);
    spin_lock_init(&d->page_alloc_lock);
    INIT_LIST_HEAD(&d->page_list);
    INIT_LIST_HEAD(&d->xenpage_list);

    rangeset_domain_initialise(d);

    if ( !is_idle_domain(d) )
    {
        set_bit(_DOMF_ctrl_pause, &d->domain_flags);
        if ( evtchn_init(d) != 0 )
            goto fail1;
        if ( grant_table_create(d) != 0 )
            goto fail2;
    }

    if ( arch_domain_create(d) != 0 )
        goto fail3;

    if ( (v = alloc_vcpu(d, 0, cpu)) == NULL )
        goto fail4;

    d->iomem_caps = rangeset_new(d, "I/O Memory", RANGESETF_prettyprint_hex);
    d->irq_caps   = rangeset_new(d, "Interrupts", 0);
    if ( (d->iomem_caps == NULL) || (d->irq_caps == NULL) )
        goto fail4; /* NB. alloc_vcpu() is undone in free_domain() */

#if 0
    if ( sched_init_domain(d) != 0 )
       goto fail4;
#endif

    if ( !is_idle_domain(d) )
    {
        write_lock(&domlist_lock);
        pd = &domain_list; /* NB. domain_list maintained in order of dom_id. */
        for ( pd = &domain_list; *pd != NULL; pd = &(*pd)->next_in_list )
            if ( (*pd)->domain_id > d->domain_id )
                break;
        d->next_in_list = *pd;
        *pd = d;
        d->next_in_hashbucket = domain_hash[DOMAIN_HASH(dom_id)];
        domain_hash[DOMAIN_HASH(dom_id)] = d;
        write_unlock(&domlist_lock);
    }

    return d;

 fail4:
    arch_domain_destroy(d);
 fail3:
    if ( !is_idle_domain(d) )
        grant_table_destroy(d);
 fail2:
    if ( !is_idle_domain(d) )
        evtchn_destroy(d);
 fail1:
    rangeset_domain_destroy(d);
    free_domain(d);
    return NULL;
}


struct domain *find_domain_by_id(domid_t dom)
{
    struct domain *d;

    read_lock(&domlist_lock);
    d = domain_hash[DOMAIN_HASH(dom)];
    while ( d != NULL )
    {
        if ( d->domain_id == dom )
        {
            if ( unlikely(!get_domain(d)) )
                d = NULL;
            break;
        }
        d = d->next_in_hashbucket;
    }
    read_unlock(&domlist_lock);

    return d;
}


void domain_kill(struct domain *d)
{
    struct vcpu *v;

    domain_pause(d);
    if ( !test_and_set_bit(_DOMF_dying, &d->domain_flags) )
    {
        for_each_vcpu(d, v)
            sched_rem_domain(v);
        gnttab_release_mappings(d);
        domain_relinquish_resources(d);
        put_domain(d);

        send_guest_virq(dom0->vcpu[0], VIRQ_DOM_EXC);
    }
}


void __domain_crash(struct domain *d)
{
    if ( d == current->domain )
    {
        printk("Domain %d (vcpu#%d) crashed on cpu#%d:\n",
               d->domain_id, current->vcpu_id, smp_processor_id());
        show_registers(guest_cpu_user_regs());
    }
    else
    {
        printk("Domain %d reported crashed by domain %d on cpu#%d:\n",
               d->domain_id, current->domain->domain_id, smp_processor_id());
    }

    domain_shutdown(d, SHUTDOWN_crash);
}


void __domain_crash_synchronous(void)
{
    __domain_crash(current->domain);
    for ( ; ; )
        do_softirq();
}


static struct domain *domain_shuttingdown[NR_CPUS];

static void domain_shutdown_finalise(void)
{
    struct domain *d;
    struct vcpu *v;

    d = domain_shuttingdown[smp_processor_id()];
    domain_shuttingdown[smp_processor_id()] = NULL;

    BUG_ON(d == NULL);
    BUG_ON(d == current->domain);

    LOCK_BIGLOCK(d);

    /* Make sure that every vcpu is descheduled before we finalise. */
    for_each_vcpu ( d, v )
        vcpu_sleep_sync(v);
    BUG_ON(!cpus_empty(d->domain_dirty_cpumask));

    sync_pagetable_state(d);

    /* Don't set DOMF_shutdown until execution contexts are sync'ed. */
    if ( !test_and_set_bit(_DOMF_shutdown, &d->domain_flags) )
        send_guest_virq(dom0->vcpu[0], VIRQ_DOM_EXC);

    UNLOCK_BIGLOCK(d);

    put_domain(d);
}

static __init int domain_shutdown_finaliser_init(void)
{
    open_softirq(DOMAIN_SHUTDOWN_FINALISE_SOFTIRQ, domain_shutdown_finalise);
    return 0;
}
__initcall(domain_shutdown_finaliser_init);


void domain_shutdown(struct domain *d, u8 reason)
{
    struct vcpu *v;

    if ( d->domain_id == 0 )
    {
        extern void machine_restart(char *);
        extern void machine_halt(void);

        debugger_trap_immediate();

        if ( reason == SHUTDOWN_poweroff ) 
        {
            printk("Domain 0 halted: halting machine.\n");
            machine_halt();
        }
        else if ( reason == SHUTDOWN_crash )
        {
            printk("Domain 0 crashed: rebooting machine in 5 seconds.\n");
            watchdog_disable();
            //mdelay(5000);
            machine_restart(0);
        }
        else
        {
            printk("Domain 0 shutdown: rebooting machine.\n");
            machine_restart(0);
        }
    }

    /* Mark the domain as shutting down. */
    d->shutdown_code = reason;

    /* Put every vcpu to sleep, but don't wait (avoids inter-vcpu deadlock). */
    for_each_vcpu ( d, v )
    {
        atomic_inc(&v->pausecnt);
        vcpu_sleep_nosync(v);
    }

    get_knownalive_domain(d);
    domain_shuttingdown[smp_processor_id()] = d;
    raise_softirq(DOMAIN_SHUTDOWN_FINALISE_SOFTIRQ);
}


void domain_pause_for_debugger(void)
{
    struct domain *d = current->domain;
    struct vcpu *v;

    /*
     * NOTE: This does not synchronously pause the domain. The debugger
     * must issue a PAUSEDOMAIN command to ensure that all execution
     * has ceased and guest state is committed to memory.
     */
    set_bit(_DOMF_ctrl_pause, &d->domain_flags);
    for_each_vcpu ( d, v )
        vcpu_sleep_nosync(v);

    send_guest_virq(dom0->vcpu[0], VIRQ_DEBUGGER);
}


/* Release resources belonging to task @p. */
void domain_destroy(struct domain *d)
{
    struct domain **pd;
    atomic_t      old, new;

    BUG_ON(!test_bit(_DOMF_dying, &d->domain_flags));

    /* May be already destroyed, or get_domain() can race us. */
    _atomic_set(old, 0);
    _atomic_set(new, DOMAIN_DESTROYED);
    old = atomic_compareandswap(old, new, &d->refcnt);
    if ( _atomic_read(old) != 0 )
        return;

    /* Delete from task list and task hashtable. */
    write_lock(&domlist_lock);
    pd = &domain_list;
    while ( *pd != d ) 
        pd = &(*pd)->next_in_list;
    *pd = d->next_in_list;
    pd = &domain_hash[DOMAIN_HASH(d->domain_id)];
    while ( *pd != d ) 
        pd = &(*pd)->next_in_hashbucket;
    *pd = d->next_in_hashbucket;
    write_unlock(&domlist_lock);

    rangeset_domain_destroy(d);

    evtchn_destroy(d);
    grant_table_destroy(d);

    arch_domain_destroy(d);

    free_domain(d);

    send_guest_virq(dom0->vcpu[0], VIRQ_DOM_EXC);
}

void vcpu_pause(struct vcpu *v)
{
    BUG_ON(v == current);
    atomic_inc(&v->pausecnt);
    vcpu_sleep_sync(v);
}

void vcpu_pause_nosync(struct vcpu *v)
{
	atomic_inc(&v->pausecnt);
	vcpu_sleep_nosync(v);
}

void domain_pause(struct domain *d)
{
    struct vcpu *v;

    for_each_vcpu( d, v )
        vcpu_pause(v);

    sync_pagetable_state(d);
}

void vcpu_unpause(struct vcpu *v)
{
    BUG_ON(v == current);
    if ( atomic_dec_and_test(&v->pausecnt) )
        vcpu_wake(v);
}

void domain_unpause(struct domain *d)
{
    struct vcpu *v;

    for_each_vcpu( d, v )
        vcpu_unpause(v);
}

void domain_pause_by_systemcontroller(struct domain *d)
{
    struct vcpu *v;

    BUG_ON(current->domain == d);

    if ( !test_and_set_bit(_DOMF_ctrl_pause, &d->domain_flags) )
    {
        for_each_vcpu ( d, v )
            vcpu_sleep_sync(v);
    }

    sync_pagetable_state(d);
}

void domain_unpause_by_systemcontroller(struct domain *d)
{
    struct vcpu *v;

    if ( test_and_clear_bit(_DOMF_ctrl_pause, &d->domain_flags) )
    {
        for_each_vcpu ( d, v )
            vcpu_wake(v);
    }
}


/*
 * set_info_guest is used for final setup, launching, and state modification 
 * of domains other than domain 0. ie. the domains that are being built by 
 * the userspace dom0 domain builder.
 */
int set_info_guest(struct domain *d, dom0_setvcpucontext_t *setvcpucontext)
{
    int rc = 0;
    struct vcpu_guest_context *c = NULL;
    unsigned long vcpu = setvcpucontext->vcpu;
    struct vcpu *v; 

    if ( (vcpu >= MAX_VIRT_CPUS) || ((v = d->vcpu[vcpu]) == NULL) )
        return -EINVAL;
    
    if ( (c = xmalloc(struct vcpu_guest_context)) == NULL )
        return -ENOMEM;

    domain_pause(d);

    rc = -EFAULT;
    if ( copy_from_guest(c, setvcpucontext->ctxt, 1) == 0 )
        rc = arch_set_info_guest(v, c);

    domain_unpause(d);

    xfree(c);
    return rc;
}

int boot_vcpu(struct domain *d, int vcpuid, struct vcpu_guest_context *ctxt) 
{
    struct vcpu *v = d->vcpu[vcpuid];

    BUG_ON(test_bit(_VCPUF_initialised, &v->vcpu_flags));

    return arch_set_info_guest(v, ctxt);
}

long do_vcpu_op(int cmd, int vcpuid, XEN_GUEST_HANDLE(void) arg)
{
    struct domain *d = current->domain;
    struct vcpu *v;
    struct vcpu_guest_context *ctxt;
    long rc = 0;

    if ( (vcpuid < 0) || (vcpuid >= MAX_VIRT_CPUS) )
        return -EINVAL;

    if ( (v = d->vcpu[vcpuid]) == NULL )
        return -ENOENT;

    switch ( cmd )
    {
    case VCPUOP_initialise:
        if ( (ctxt = xmalloc(struct vcpu_guest_context)) == NULL )
        {
            rc = -ENOMEM;
            break;
        }

        if ( copy_from_guest(ctxt, arg, 1) )
        {
            xfree(ctxt);
            rc = -EFAULT;
            break;
        }

        LOCK_BIGLOCK(d);
        rc = -EEXIST;
        if ( !test_bit(_VCPUF_initialised, &v->vcpu_flags) )
            rc = boot_vcpu(d, vcpuid, ctxt);
        UNLOCK_BIGLOCK(d);

        xfree(ctxt);
        break;

    case VCPUOP_up:
        if ( !test_bit(_VCPUF_initialised, &v->vcpu_flags) )
            rc = -EINVAL;
        else if ( test_and_clear_bit(_VCPUF_down, &v->vcpu_flags) )
            vcpu_wake(v);
        break;

    case VCPUOP_down:
        if ( !test_and_set_bit(_VCPUF_down, &v->vcpu_flags) )
            vcpu_sleep_nosync(v);
        break;

    case VCPUOP_is_up:
        rc = !test_bit(_VCPUF_down, &v->vcpu_flags);
        break;

    case VCPUOP_get_runstate_info:
    {
        struct vcpu_runstate_info runstate;
        vcpu_runstate_get(v, &runstate);
        if ( copy_to_guest(arg, &runstate, 1) )
            rc = -EFAULT;
        break;
    }
    case VCPUOP_set_periodic_timer:
    {
        struct vcpu_set_periodic_timer set;

        if ( copy_from_guest(&set, arg, 1) ) {
            return -EFAULT;
        }

        if ( set.period_ns < MILLISECS(1) ) {
            return -EINVAL;
        }

        v->periodic_timer_period = set.period_ns;
#if 1
        vcpu_force_reschedule(v);
#endif

        break;
    }
    case VCPUOP_stop_periodic_timer:
    {
        v->periodic_timer_period = 0;
        vcpu_force_reschedule(v);

        break;
    }
    case VCPUOP_set_singleshot_timer:
    {
#if 0
        struct vcpu_set_singleshot_timer set;

        if ( v != current )
            return -EINVAL;

        if ( copy_from_guest(&set, arg, 1) )
            return -EFAULT;

        if ( v->singleshot_timer.cpu != smp_processor_id() )
        {
            stop_timer(&v->singleshot_timer);
            v->singleshot_timer.cpu = smp_processor_id();
        }

        set_timer(&v->singleshot_timer, NOW() + set.timeout_abs_ns);
#endif
        break;
    }
    case VCPUOP_stop_singleshot_timer:
    {
        if ( v != current )
            return -EINVAL;

        stop_timer(&v->singleshot_timer);

        break;
    }

    default:
        rc = arch_do_vcpu_op(cmd, v, arg);
        break;
    }

    return rc;
}

long vm_assist(struct domain *p, unsigned int cmd, unsigned int type)
{
    if ( type > MAX_VMASST_TYPE )
        return -EINVAL;

    switch ( cmd )
    {
    case VMASST_CMD_enable:
        set_bit(type, &p->vm_assist);
        return 0;
    case VMASST_CMD_disable:
        clear_bit(type, &p->vm_assist);
        return 0;
    }

    return -ENOSYS;
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
